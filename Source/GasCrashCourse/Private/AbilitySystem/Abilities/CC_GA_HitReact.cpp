// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/CC_GA_HitReact.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/CC_BaseCharacter.h"
#include "Tags/CC_GameplayTags.h"

UCC_GA_HitReact::UCC_GA_HitReact():Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UCC_GA_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//Only trigger once should be false in order to retrigger this ability
	WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CCTags::Event::HitReact, nullptr, false,true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &UCC_GA_HitReact::OnEventReceived);
	WaitGameplayEventTask->ReadyForActivation();
}

void UCC_GA_HitReact::CC_CancelAbility()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UCC_GA_HitReact::CleanUpAndEndAbility(const bool bWasCanceled)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCanceled);
}

void UCC_GA_HitReact::OnEventReceived(FGameplayEventData Payload)
{
	if (HitReactVFX && IsValid(OwnerCharacter.Get()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitReactVFX, OwnerCharacter.Get()->GetActorLocation());
	}
	if (!IsValid(OwnerCharacter->GetMesh()->GetAnimInstance()))
	{
		UE_LOG(LogTemp,Warning, TEXT("Unable to retrieve anim instance"))
		return;
	}
	if (!IsValid(HitReactMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReactMontage was not provided"))
		return;
	}
	auto Instigator = Payload.Instigator;
	const EHitDirection HitDirection = UCC_GameplayStatics::GetHitDirectionFromInstigator(GetAvatarActorFromActorInfo(),Instigator , true);
	if (bDebug)
	{
		FString Msg = FString::Printf(TEXT("Direction: %s") ,*StaticEnum<EHitDirection>()->GetNameByValue(static_cast<int64>(HitDirection)).ToString());
		UCC_GameplayStatics::PrintScreenDebugMessage(Msg);
	}
	const FName MontageSection = GetMontageSectionName(HitDirection);
	//Play Montage
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HitReactMontage,1.f,MontageSection,true);
	MontageTask->OnBlendOut.AddDynamic(this, &UCC_GA_HitReact::OnBlendOut);
	MontageTask->OnInterrupted.AddDynamic(this, &UCC_GA_HitReact::OnInterrupted);
	MontageTask->OnCompleted.AddDynamic(this, &UCC_GA_HitReact::OnCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UCC_GA_HitReact::OnCancelled);
	MontageTask->ReadyForActivation();
}

FName UCC_GA_HitReact::GetMontageSectionName(EHitDirection HitDirection)const{
	switch (HitDirection)
	{
		case EHitDirection::Back: return FName("Back");
		case EHitDirection::Left: return FName("Left");
		case EHitDirection::Right: return FName("Right");
		case EHitDirection::Front: return FName("Front");
		default: return NAME_None;
	}
}

void UCC_GA_HitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (MontageTask && MontageTask->IsActive())
	{
		MontageTask->EndTask();
	}
	if (WaitGameplayEventTask && WaitGameplayEventTask->IsActive())
	{
		WaitGameplayEventTask->EndTask();
	}
	MontageTask = nullptr;
	WaitGameplayEventTask = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}
void UCC_GA_HitReact::OnBlendOut()
{
	//CleanUpAndEndAbility(false);
}

void UCC_GA_HitReact::OnInterrupted()
{
	//CleanUpAndEndAbility(true);
}

void UCC_GA_HitReact::OnCancelled()
{
	//CleanUpAndEndAbility(true);
}

void UCC_GA_HitReact::OnCompleted()
{
	// CleanUpAndEndAbility(false);
}