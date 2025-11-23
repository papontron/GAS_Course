// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CC_AbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="CC|Ability")
	void IncreaseAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Quantity=1);
	UFUNCTION(BlueprintCallable, Category ="CC|Ability")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 AbilityLevel);

	
protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
private:
	void HandleAutoActivateAbility(const FGameplayAbilitySpec& AbilitySPec);
};
