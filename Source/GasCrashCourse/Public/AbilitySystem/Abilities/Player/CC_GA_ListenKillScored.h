// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_GA_ListenKillScored.generated.h"

class UAbilityTask_WaitGameplayEvent;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_GA_ListenKillScored : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_GA_ListenKillScored();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = nullptr;

	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);
};
