// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "CC_GameplayAbility.h"
#include "Utils/CC_GameplayStatics.h"
#include "CC_GA_HitReact.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UNiagaraSystem;



UCLASS()
class GASCRASHCOURSE_API UCC_GA_HitReact : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_GA_HitReact();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
#pragma region Setup
	UPROPERTY(EditDefaultsOnly, Category="Setup|Montage")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category="Setup|VFX")
	TObjectPtr<UNiagaraSystem> HitReactVFX;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Debug")
	bool bDebug = false;
#pragma endregion
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = nullptr;
	void CC_CancelAbility();
	void CleanUpAndEndAbility(const bool bWasCanceled);

	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

	FName GetMontageSectionName(EHitDirection HitDirection)const;

	UFUNCTION()
	void OnBlendOut();
	UFUNCTION()
	void OnInterrupted();
	UFUNCTION()
	void OnCancelled();
	UFUNCTION()
	void OnCompleted();
	
};
