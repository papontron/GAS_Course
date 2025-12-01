// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/CC_GA_Death.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/CC_GameplayTags.h"
#include "Utils/CC_GameplayStatics.h"

UCC_GA_Death::UCC_GA_Death():Super()
{
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	FGameplayTagContainer Tags;
	Tags.AddTag(CCTags::Ability::Death);
	SetAssetTags(Tags);
	FGameplayTagContainer CancelTags;
	CancelTags.AddTag(FGameplayTag::RequestGameplayTag("CCTags.Ability"));
	BlockAbilitiesWithTag = CancelTags;
	CancelAbilitiesWithTag = CancelTags;
}

void UCC_GA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CurrentMontageIndex = UCC_GameplayStatics::GetRandomArrayIndex(DeathMontageArray.Num());
	DeathMontageArray[CurrentMontageIndex]->bEnableAutoBlendOut = false;
	PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None,DeathMontageArray[CurrentMontageIndex], 1.f);
	PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &ThisClass::OnBlendOut);
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &ThisClass::OnCancelled);
	PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterrupted);
	PlayMontageAndWaitTask->ReadyForActivation();
	//add the death tag to ASC
	FGameplayTagContainer Tags;
	Tags.AddTag(CCTags::Status::Dead);
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTags(Tags);
}

void UCC_GA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (PlayMontageAndWaitTask && PlayMontageAndWaitTask->IsActive())
	{
		PlayMontageAndWaitTask->EndTask();
	}
	PlayMontageAndWaitTask = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCC_GA_Death::OnBlendOut()
{
	CleanUpAndEndAbility(false);
}

void UCC_GA_Death::OnCancelled()
{
	CleanUpAndEndAbility(true);	
}

void UCC_GA_Death::OnInterrupted()
{
	CleanUpAndEndAbility(true);
}

void UCC_GA_Death::OnCompleted()
{
	CleanUpAndEndAbility(false);
}

