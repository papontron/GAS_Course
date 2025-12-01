// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/Enemy/CC_AttackTarget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Characters/CC_Enemycharacter.h"
#include "Tags/CC_GameplayTags.h"
#include "Utils/CC_GameplayStatics.h"

UCC_AttackTarget::UCC_AttackTarget()
{
	FGameplayTagContainer AssetTags = GetAssetTags();
	AssetTags.AddTag(CCTags::Ability::Enemy::AttackTarget);
	SetAssetTags(AssetTags);
}

void UCC_AttackTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	EnemyOwner = Cast<ACC_Enemycharacter>(OwnerCharacter);
	if (!EnemyOwner.IsValid() || !IsValid(EnemyOwner->GetTargetActor()) || !GetWorld())
	{
		return GoBackToSearchTargetAndCleanUp();
	}
	FTimerDelegate TryAttackLoopDelegate;
	TryAttackLoopDelegate.BindUObject(this, &ThisClass::PlayTryAttackLoop);
	const float AttackRate = FMath::FRandRange(EnemyOwner->GetMinAttackRate(), EnemyOwner->GetMaxAttackRate());
	GetWorld()->GetTimerManager().SetTimer(TryAttackTimerHandle, TryAttackLoopDelegate,AttackRate, true, AttackRate);
}

void UCC_AttackTarget::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCC_AttackTarget::AttackTarget()
{
	if (!EnemyOwner.IsValid())
	{
		return;
	}
	UCC_GameplayStatics::PrintScreenDebugMessage(FString("Attacking from GA"));
	FGameplayEventData Payload;
	Payload.Instigator = EnemyOwner->GetTargetActor();
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), CCTags::Event::Enemy::PrimaryAttack, Payload);
	
	UAbilitySystemComponent* ASC = EnemyOwner->GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;
	FGameplayTag AbilityTagToActivate = CCTags::Ability::Enemy::Primary;
	ASC->TryActivateAbilitiesByTag(AbilityTagToActivate.GetSingleTagContainer());
}

void UCC_AttackTarget::OnDelayFinished() //After rotation
{
	AttackTarget();
}





void UCC_AttackTarget::PlayTryAttackLoop()
{
	//check for the distance to the target in every loop cicle
	const float DistanceToTarget  = FVector::Dist(EnemyOwner->GetActorLocation(), EnemyOwner->GetTargetActor()->GetActorLocation());
	if (DistanceToTarget <= EnemyOwner->GetAttackRange())
	{
		if (WaitDelayTask && WaitDelayTask->IsActive())
		{
			WaitDelayTask->EndTask();
			WaitDelayTask = nullptr;
		}
		//if within attack range, attack:
		EnemyOwner->RotateTowardsTarget();
		const float Delay = EnemyOwner->GetTimelineLength();
		WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this,Delay );
		WaitDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnDelayFinished);
		WaitDelayTask->ReadyForActivation();
		// if (GetWorld())
		// {
		// 	GetWorld()->GetTimerManager().PauseTimer(TryAttackTimerHandle);
		// }
	}else
	{
		//if target is out of range, check if the actor is still within awareness range
		if (DistanceToTarget <= EnemyOwner->GetSearchRadius())
		{
			//go back to move to state
			ActivateMoveToLocationAbilityAndCleanUp();
		}else
		{
			GoBackToSearchTargetAndCleanUp();
		}
	}
}

void UCC_AttackTarget::ActivateMoveToLocationAbilityAndCleanUp()
{
	FGameplayTag MoveToLocationAbilityTag = CCTags::Ability::Enemy::MoveToLocation;
	if (!GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(MoveToLocationAbilityTag.GetSingleTagContainer()))
	{
		GoBackToSearchTargetAndCleanUp();
	}else
	{
		CleanUpAndEndAbility(false);
	}
}

void UCC_AttackTarget::GoBackToSearchTargetAndCleanUp()
{
	FGameplayTag SearchTargetAbilityTag = CCTags::Ability::Enemy::SearchTarget;
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(SearchTargetAbilityTag.GetSingleTagContainer());
	CleanUpAndEndAbility(false);
}
