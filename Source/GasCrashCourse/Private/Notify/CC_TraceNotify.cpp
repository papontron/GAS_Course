// Papontron Inc. All Rigths reserved.


#include "Notify/CC_TraceNotify.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interface/PlayerCharacterInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CC_GameplayStatics.h"




void UCC_TraceNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (!IsValid(MeshComp)) return;
	auto Owner = MeshComp->GetOwner();
	if (!IsValid(Owner)) return;
	TArray<FHitResult> HitResults;
	TraceSphereMultiChannel(MeshComp, HitResults);
	if (HitResults.Num() == 0) return;
	SendEventToActors(HitResults, Owner);
}

void UCC_TraceNotify::TraceSphereMultiChannel(const USkeletalMeshComponent* MeshComp, TArray<FHitResult>& HitResults)
{
	auto Socket = MeshComp->GetSocketByName(SocketName);
	if (!IsValid(Socket)) return;
	FTransform Transform = MeshComp->GetSocketTransform(SocketName);
	FRotator SocketRotation = Transform.GetRotation().Rotator();
	FVector Start = Transform.GetLocation();
	FVector OffSet = UKismetMathLibrary::GetForwardVector(SocketRotation)*100;
	FVector End = Start - OffSet;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MeshComp->GetOwner());
	EDrawDebugTrace::Type DrawDebugTrace = bDebug? EDrawDebugTrace::ForDuration:EDrawDebugTrace::None;
	UKismetSystemLibrary::SphereTraceMulti(MeshComp->GetOwner(),
		Start, End,TraceRadius,
		UEngineTypes::ConvertToTraceType(HitTraceChannel), false, ActorsToIgnore,DrawDebugTrace, HitResults, true);
}

void UCC_TraceNotify::SendEventToActors(TArray<FHitResult>& HitResults, AActor* Owner)
{
	for (FHitResult& HitResult : HitResults)
	{
		if (!HitResult.bBlockingHit) continue;
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) && HitActor->Implements<UAbilitySystemInterface>())
		{
			IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(HitActor);
			if (auto ASC =AbilityInterface->GetAbilitySystemComponent())
			{
				FGameplayEventData Payload;
				Payload.Target = HitActor;
				Payload.Instigator = Owner;
				FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
				ContextHandle.AddHitResult(HitResult);
				Payload.ContextHandle = ContextHandle;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, EventTag, Payload);
			}
		}
	}
}
