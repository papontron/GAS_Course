// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/Player/CC_GA_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Tags/CC_GameplayTags.h"
#include "Utils/CC_GameplayStatics.h"

UCC_GA_Primary::UCC_GA_Primary():Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
}

void UCC_GA_Primary::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//Here it could be possible to add Blocking Tags:
	// FGameplayTagContainer BlockingTags;
	// BlockingTags.AddTag(CCTags::Ability::Player::Primary);
	// GetAbilitySystemComponentFromActorInfo()->ApplyAbilityBlockAndCancelTags()
	if (!PrimaryAttackMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("Primary Attack Montage was not set"))
		EndAbility(Handle,ActorInfo,ActivationInfo, true, false);
	}
	if (!EventTagToWaitFor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Event Tag to wait was not set"))
		EndAbility(Handle,ActorInfo,ActivationInfo, true, false);
	}
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Primary Attack: No Anim Instance found"));
		EndAbility(Handle,ActorInfo,ActivationInfo, true, false);
	}
	FName MontageSection = GetRandomMontageSectionNameFromArray();
	PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PrimaryAttackMontage,1,MontageSection);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UCC_GA_Primary::OnMontageFinished);
	PlayMontageTask->OnBlendOut.AddDynamic(this,&ThisClass::OnMontageBlendOut);
	PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	PlayMontageTask->OnInterrupted.AddDynamic(this,&ThisClass::OnMontageInterrupted);
	PlayMontageTask->ReadyForActivation();

	WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTagToWaitFor,nullptr,true,true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this,&ThisClass::OnGameplayEventReceived);
	WaitGameplayEventTask->ReadyForActivation();
}

void UCC_GA_Primary::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	//cleanup tasks:
	if (PlayMontageTask && PlayMontageTask->IsActive())
	{
		PlayMontageTask->EndTask();
	}
	if (WaitGameplayEventTask && WaitGameplayEventTask->IsActive())
	{
		WaitGameplayEventTask->EndTask();
	}
	PlayMontageTask = nullptr;
	WaitGameplayEventTask = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCC_GA_Primary::CleanUpAndEndAbility()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo, true, false);
}



void UCC_GA_Primary::OnMontageFinished()
{
	CleanUpAndEndAbility();
}

void UCC_GA_Primary::OnMontageInterrupted()
{
	CC_CancelAbility();
}

void UCC_GA_Primary::OnMontageBlendOut()
{
	CC_CancelAbility();
}



void UCC_GA_Primary::OnGameplayEventReceived( FGameplayEventData Payload)
{
	TraceHitSphere();
}

void UCC_GA_Primary::OnMontageCancelled()
{
	CC_CancelAbility();
}
void UCC_GA_Primary::TraceHitSphere()
{
	const FVector Start = GetAvatarActorFromActorInfo()->GetActorLocation() + GetAvatarActorFromActorInfo()->GetActorForwardVector()*SphereCenterOffSet + FVector(0,0,VerticalOffset);
	if (bDebug)
	{
		UCC_GameplayStatics::DrawDebugSphere(this, Start,SphereRadius);
	}
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		Start,Start,
		SphereRadius,
		UEngineTypes::ConvertToTraceType(HitCollisionChannel),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,HitResults, true);
	for (const FHitResult& HitResult : HitResults)
	{
		if (PrimaryAttackVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),PrimaryAttackVFX, HitResult.ImpactPoint);
		}
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo();
		// UGameplayAbility::SendGameplayEvent(CCTags::Event::HitReact ,Payload);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), CCTags::Event::HitReact, Payload);
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor()))
		{
			CC_ApplyGameplayEffectToTarget(PrimaryDamageGameplayEffect, TargetASC);
		}
	}
}

FName UCC_GA_Primary::GetRandomMontageSectionNameFromArray()
{
	if (MontageSections.Num()==0)
	{
		return NAME_None;	
	}
	const int32 RandomIndex = FMath::RandRange(0,MontageSections.Num()-1);
	return MontageSections[RandomIndex];
}

void UCC_GA_Primary::CC_CancelAbility()
{
	CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo, true);
}
