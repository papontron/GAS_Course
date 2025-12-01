// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/Enemy/CC_SearchTarget.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Characters/CC_Enemycharacter.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Tags/CC_GameplayTags.h"
#include "Utils/CC_GameplayStatics.h"

UCC_SearchTarget::UCC_SearchTarget()
{
	FGameplayTagContainer NewAssetTags = GetAssetTags();
	NewAssetTags.AddTag(CCTags::Ability::AutoActivate);
	NewAssetTags.AddTag(CCTags::Ability::Enemy::SearchTarget);
	SetAssetTags(NewAssetTags);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UCC_SearchTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//Cache the owner:
	EnemyOwner = Cast<ACC_Enemycharacter>(OwnerCharacter);
	//Search for Target
	StartSearching();
}

void UCC_SearchTarget::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SearchTargetTimerHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}



void UCC_SearchTarget::StartSearching()
{
	if (!GetWorld()|| !EnemyOwner.IsValid()) return;
	GetWorld()->GetTimerManager().ClearTimer(SearchTargetTimerHandle);
	FTimerDelegate SearchTargetDelegate;
	SearchTargetDelegate.BindUObject(this, &ThisClass::FindClosestTarget);
	const float SearchRate = EnemyOwner->GetSearchRate();
	GetWorld()->GetTimerManager().SetTimer(SearchTargetTimerHandle, SearchTargetDelegate,SearchRate, true);
}



void UCC_SearchTarget::FindClosestTarget()
{
	if (bDebug) UCC_GameplayStatics::PrintScreenDebugMessage(FString("trying to find a Target.."));
	if (!EnemyOwner.IsValid())
	{
		if (bDebug) UCC_GameplayStatics::PrintScreenDebugMessage(FString("Invalid Enemy Owner, Terminating SearchTarget Ability"));
		return CleanUpAndEndAbility(true);
	}
	FFindClosestTargetResult FindResult = UCC_GameplayStatics::FindClosesTargetWithTag(EnemyOwner.Get(), EnemyOwner.Get()->GetActorLocation(), PlayerTag::PlayerTag);
	if (FindResult.Target.IsValid())
	{
		if (bDebug) UCC_GameplayStatics::PrintScreenDebugMessage(FString("Found Target.."));
		//if a target is found
		//1) Set Target ACtor:
		EnemyOwner->SetTargetActor(FindResult.Target.Get());
		//2)Activate Move Ability
		ActivateMoveToLocationAbilityAndCleanUp();
	}
}


void UCC_SearchTarget::ActivateMoveToLocationAbilityAndCleanUp()
{
	FGameplayTagContainer TagsToActivate;
	TagsToActivate.AddTag(CCTags::Ability::Enemy::MoveToLocation);
	if (GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(TagsToActivate))
	{
		if (bDebug)
		{
			UCC_GameplayStatics::PrintScreenDebugMessage(FString::Printf(TEXT("MoveToLocation Was Activated")));
		}
		CleanUpAndEndAbility(false);
	}else
	{
		if (bDebug) UCC_GameplayStatics::PrintScreenDebugMessage(FString("Failed to Activate MoveToLocation Ability"));
	}
	
}
