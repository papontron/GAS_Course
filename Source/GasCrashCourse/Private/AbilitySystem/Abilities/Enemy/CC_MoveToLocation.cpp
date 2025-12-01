// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/Enemy/CC_MoveToLocation.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Characters/CC_Enemycharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tags/CC_GameplayTags.h"
#include "Tasks/AITask_MoveTo.h"
#include "Utils/CC_GameplayStatics.h"

UCC_MoveToLocation::UCC_MoveToLocation()
{	
	FGameplayTagContainer AssetTags = GetAssetTags();
	AssetTags.AddTag(CCTags::Ability::Enemy::MoveToLocation);
	SetAssetTags(AssetTags);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
}

void UCC_MoveToLocation::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	EnemyOwner = Cast<ACC_Enemycharacter>(OwnerCharacter);
	if (EnemyOwner.IsValid())
	{
		if (bDebug) UCC_GameplayStatics::PrintScreenDebugMessage(FString::Printf(TEXT("%s: Invalid Enemy Owner"), *GetName()));
		EnemyController = Cast<AAIController>(EnemyOwner->GetController());
	}
	
	MoveToLocation();
}

void UCC_MoveToLocation::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(MoveToTask) && MoveToTask -> IsActive())
	{
		MoveToTask->EndTask();
		MoveToTask = nullptr;
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCC_MoveToLocation::MoveToLocation()
{
	if (!EnemyOwner.IsValid() || !IsValid(EnemyOwner->GetTargetActor()) || !EnemyController.IsValid())
	{
		//if the owner or the target aren't valid, then end the ability
		if (bDebug)
		{
			UCC_GameplayStatics::PrintScreenDebugMessage(FString("Canceling MoveToLocation"));
			UCC_GameplayStatics::PrintScreenDebugMessage(FString::Printf(TEXT("Invalid EnemyOwner or TargetActor or EnemyController")));
		}
		GoBackToSearchAndCleanUp();
	}else
	{
		FAIMoveRequest Request;
		FVector GoalLocation = EnemyOwner->GetTargetActor()->GetActorLocation();
		GoalLocation.Z = 0;
		FString Message = FString::Printf(TEXT("Goal Location: %f, %f, %f"), GoalLocation.X, GoalLocation.Y, GoalLocation.Z);
		UCC_GameplayStatics::PrintScreenDebugMessage(Message);
		Request.SetGoalLocation(GoalLocation);
		const float AcceptanceRadius = FMath::FRandRange(EnemyOwner->GetRadiusAcceptance(),EnemyOwner->GetAttackRange());
		Request.SetAcceptanceRadius(AcceptanceRadius);
		MoveToTask = UAITask_MoveTo::AIMoveTo(EnemyController.Get(), EnemyOwner->GetTargetActor()->GetActorLocation(), nullptr, AcceptanceRadius);
		MoveToTask->OnMoveTaskFinished.AddUObject(this, &ThisClass::OnMoveFinished);
		MoveToTask->ConditionalPerformMove();
		//MoveToTask->ReadyForActivation();
	}
}


void UCC_MoveToLocation::OnMoveFinished(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	TryActivateAttackAbilityAndCleanUp();	
}

void UCC_MoveToLocation::TryActivateAttackAbilityAndCleanUp()
{
	if (MoveToTask->WasMoveSuccessful())
	{
		//activate attack ability
		FGameplayTagContainer TagsToActivate;
		TagsToActivate.AddTag(CCTags::Ability::Enemy::AttackTarget);
		if (GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(TagsToActivate))
		{
			//if attack ability was activated, then end this ability
			CleanUpAndEndAbility(false);
		}else
		{
			GoBackToSearchAndCleanUp();
		}
	}else{
		//search again
		GoBackToSearchAndCleanUp();
	}
}

void UCC_MoveToLocation::GoBackToSearchAndCleanUp()
{
	//go back to search
	FGameplayTag SearchTargetAbilityTag = CCTags::Ability::Enemy::SearchTarget;
	if (GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(SearchTargetAbilityTag.GetSingleTagContainer()))
	{
		CleanUpAndEndAbility(true);
	}else 
	{
		if (bDebug) UCC_GameplayStatics::PrintScreenDebugMessage(FString::Printf(TEXT("%s: Failed to activate search ability"), *GetName()));	
	}
	
}
