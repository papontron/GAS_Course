// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_GA_RangedPrimary.generated.h"

class ACC_Enemycharacter;
class UTimelineComponent;
class ACC_PlayerCharacter;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_GA_RangedPrimary : public UCC_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UCC_GA_RangedPrimary();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
#pragma region Setup
	UPROPERTY(EditDefaultsOnly, Category="Setup|Montage")
	TObjectPtr<UAnimMontage> AttackMontage;
#pragma endregion

	UPROPERTY()
	TWeakObjectPtr<ACC_PlayerCharacter> TargetCharacter;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayMontageTask;

	UFUNCTION()
	void OnEventReceived(FGameplayEventData EventData);

	void PlayAttackMontage();
	
	

	void CleanUpAndEndAbility(bool bWasCancelled);
	//Play montage callbacks:
	UFUNCTION()
	void OnCompleted();
	UFUNCTION()
	void OnCancelled();
	UFUNCTION()
	void OnInterrupted();
	UFUNCTION()
	void OnBlendOut();


	TWeakObjectPtr<ACC_Enemycharacter> EnemyOwner; //owner of this ability
};
