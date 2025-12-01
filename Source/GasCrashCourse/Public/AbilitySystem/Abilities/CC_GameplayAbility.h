// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CC_GameplayAbility.generated.h"

class ACC_BaseCharacter;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCC_GameplayAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	TWeakObjectPtr<ACC_BaseCharacter> OwnerCharacter;
	void CC_ApplyGameplayEffectToTarget(const TSubclassOf<UGameplayEffect>& EffectClass, UAbilitySystemComponent* TargetASC) const;
	void CC_ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& EffectClass) const;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Debug")
	bool bDebug = false;

	void CleanUpAndEndAbility(bool bWasCancelled);
};


