// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_SearchTarget.generated.h"

class ACC_Enemycharacter;
class UAbilityTask_WaitDelay;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_SearchTarget : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_SearchTarget();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	TWeakObjectPtr<ACC_Enemycharacter> EnemyOwner;
	
private:
	FTimerHandle SearchTargetTimerHandle;
	
	void FindClosestTarget();
	void StartSearching();
	

	void ActivateMoveToLocationAbilityAndCleanUp();
	
};
