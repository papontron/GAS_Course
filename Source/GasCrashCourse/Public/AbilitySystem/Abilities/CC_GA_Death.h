// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "CC_GameplayAbility.h"
#include "CC_GA_Death.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * THIS ABILITY IS ACTIVATED FROM BASE_CHARACTER HANDLE_DEATH()
 */
UCLASS()
class GASCRASHCOURSE_API UCC_GA_Death : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_GA_Death();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	UFUNCTION(BlueprintCallable, Category = "CC|GameplayAbility")
	TArray<UAnimMontage*> GetDeathMontages() const{ return DeathMontageArray;}
	UFUNCTION(BlueprintCallable, Category = "CC|GameplayAbility")
	int GetCurrentMontageIndex() const{	return CurrentMontageIndex;}
	UFUNCTION(BlueprintCallable, Category="CC|GameplayAbility")
	void CleanUpAndEndAbility(const bool bWasCancelled);
private:
#pragma region Setup
	UPROPERTY(EditDefaultsOnly, Category="Setup|Montage")
	TArray<TObjectPtr<UAnimMontage>> DeathMontageArray;
	
#pragma endregion
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = nullptr;

	UFUNCTION()
	void OnBlendOut();
	UFUNCTION()
	void OnCancelled();
	UFUNCTION()
	void OnInterrupted();
	UFUNCTION()
	void OnCompleted();

	
	int CurrentMontageIndex;
};
