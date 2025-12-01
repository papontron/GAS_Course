// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_AttackTarget.generated.h"

class UAbilityTask_WaitDelay;
class UAbilityTask_PlayMontageAndWait;
class ACC_Enemycharacter;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_AttackTarget : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_AttackTarget();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	TWeakObjectPtr<ACC_Enemycharacter> EnemyOwner;
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayMontageTask;
	UPROPERTY()
	UAbilityTask_WaitDelay* WaitDelayTask;
	void AttackTarget();
	
	UFUNCTION()
	void OnDelayFinished();

	

	FTimerHandle TryAttackTimerHandle;

	void PlayTryAttackLoop();

	void ActivateMoveToLocationAbilityAndCleanUp();
	void GoBackToSearchTargetAndCleanUp();
};
