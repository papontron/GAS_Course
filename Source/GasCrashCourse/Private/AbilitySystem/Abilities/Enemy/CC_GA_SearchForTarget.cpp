// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/Enemy/CC_GA_SearchForTarget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Tags/CC_GameplayTags.h"
#include "Characters/CC_BaseCharacter.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Characters/CC_Enemycharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Player/CC_EnemyController.h"
#include "Utils/CC_GameplayStatics.h"
UCC_GA_SearchForTarget::UCC_GA_SearchForTarget()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//since this is only for ai, we need the server to be in charge of this ability
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	//must auto activate
	auto Tags = GetAssetTags();
	Tags.AddTag(CCTags::Ability::AutoActivate);
	SetAssetTags(Tags);
}

void UCC_GA_SearchForTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//first: Cast owner character to CC_EnemyCharacter
	OwnerEnemy = Cast<ACC_Enemycharacter>(OwnerCharacter);
	AIController = Cast<ACC_EnemyController>(UAIBlueprintHelperLibrary::GetAIController(OwnerEnemy.Get()));
	//set the timer
	StartFindTargetLoop();
}

void UCC_GA_SearchForTarget::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FindTargetTimerHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}



void UCC_GA_SearchForTarget::StartFindTargetLoop()
{
	if (!OwnerEnemy.IsValid()) return;
	//find a target
	FindTargetTimerDelegate.BindLambda([this]()
	{
		FindTarget();
		MoveToTargetWithinAttackRange();
		//start update engage status
		UpdateEngagedStatus();
	});
	
	float Delay = FMath::FRandRange(0.f, OwnerEnemy->GetSearchRate());
	bool bLoop = true;
	GetWorld()->GetTimerManager().SetTimer(FindTargetTimerHandle,FindTargetTimerDelegate, Delay, bLoop);
}
void UCC_GA_SearchForTarget::FindTarget()
{
	//clean update status timer:
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateEngageStatusTimerHandle);
	}
	//is the enemy is already engaged in battle, don't search another target
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(CCTags::Status::Enemy::Engaged)) return;
	FFindClosestTargetResult Result = UCC_GameplayStatics::FindClosesTargetWithTag(this, OwnerCharacter.Get()->GetActorLocation(), PlayerTag::PlayerTag);
	if (!Result.Target.IsValid()) return;
	if (Result.Distance<= OwnerEnemy.Get()->GetSearchRadius())
	{
		Target = Result.Target;
		//if a valid target is found, change enemy status to "Engaged"
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(CCTags::Status::Enemy::Engaged);
		//stop the search target TImer:
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(FindTargetTimerHandle);
		}
		//if a target is found also we want to keep attacking until it goes out of attack range
		return;
	}
	
	Target = nullptr;
}
void UCC_GA_SearchForTarget::MoveToTargetWithinAttackRange()
{
	if (!Target.IsValid() || !AIController.IsValid()) return;
	float RandomAcceptanceRadius = FMath::FRandRange(0, OwnerEnemy->GetAttackRange());
	AIController->MoveToActor(Target.Get(), RandomAcceptanceRadius);
	//after reaching the actor, send gameplay event to this actor (attack order)
	PrimaryAttackEventPayload.Instigator = Target.Get();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerEnemy.Get(), CCTags::Event::Enemy::PrimaryAttack, PrimaryAttackEventPayload);
}

void UCC_GA_SearchForTarget::UpdateEngagedStatus()
{
	//this method triggers a TImerLoop to update the Engaged status
	
	UpdateEngageStatusTimerDelegate.BindLambda([this]()
	{
		if (!Target.IsValid()) return;
		if (FVector::Dist(OwnerEnemy->GetActorLocation(), Target->GetActorLocation())> OwnerEnemy->GetAttackRange())//if the target is out of attack range
		{
			//stop the UpdateStatusEngagedTimerHandle
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(UpdateEngageStatusTimerHandle);
			}
			//clean Engaged status:
			GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(CCTags::Status::Enemy::Engaged);
			//restart the SearchTarget TimerEvent;
			float Delay = FMath::FRandRange(0, OwnerEnemy->GetSearchRate());
			GetWorld()->GetTimerManager().SetTimer(FindTargetTimerHandle, FindTargetTimerDelegate, 1.f, true, Delay);
		}else
		{
			PrimaryAttackEventPayload.Instigator = Target.Get(); //order to attack to the target actor
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerEnemy.Get(), CCTags::Event::Enemy::PrimaryAttack,PrimaryAttackEventPayload);
		}
	});
	if (GetWorld())
	{
		float Delay = FMath::FRandRange(OwnerEnemy->GetMinAttackRate(),OwnerEnemy->GetMinAttackRate());
		GetWorld()->GetTimerManager().SetTimer(UpdateEngageStatusTimerHandle, UpdateEngageStatusTimerDelegate, Delay, true, Delay);
	}
	
}

