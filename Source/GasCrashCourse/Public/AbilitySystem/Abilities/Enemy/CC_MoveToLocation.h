// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_MoveToLocation.generated.h"

namespace EPathFollowingResult
{
	enum Type: int;
}

class UAITask_MoveTo;
class ACC_Enemycharacter;
class AAIController;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_MoveToLocation : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_MoveToLocation();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	TWeakObjectPtr<AAIController> EnemyController;
	TWeakObjectPtr<ACC_Enemycharacter> EnemyOwner;
	
	UPROPERTY()
	UAITask_MoveTo* MoveToTask;
	
	void MoveToLocation();
	void OnMoveFinished(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController);
	void TryActivateAttackAbilityAndCleanUp();
	void GoBackToSearchAndCleanUp();
};
