// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_GA_SearchForTarget.generated.h"

class ACC_EnemyController;
class ACC_Enemycharacter;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_GA_SearchForTarget : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_GA_SearchForTarget();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	TWeakObjectPtr<AActor> Target;
	FTimerHandle FindTargetTimerHandle;
	FTimerDelegate FindTargetTimerDelegate;
	
	FTimerHandle UpdateEngageStatusTimerHandle;
	FTimerDelegate UpdateEngageStatusTimerDelegate;
	
	TWeakObjectPtr<ACC_Enemycharacter> OwnerEnemy;
	TWeakObjectPtr<ACC_EnemyController> AIController;

	FGameplayEventData PrimaryAttackEventPayload;
	
	void FindTarget();
	void StartFindTargetLoop();
	void MoveToTargetWithinAttackRange();
	
	void UpdateEngagedStatus();
};
