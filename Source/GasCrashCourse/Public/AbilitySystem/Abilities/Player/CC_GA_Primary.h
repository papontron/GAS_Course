// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_GA_Primary.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class ACC_PlayerCharacter;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_GA_Primary : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_GA_Primary();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
#pragma region Setup
	UPROPERTY(EditDefaultsOnly, Category="Setup|Montage")
	TObjectPtr<UAnimMontage> PrimaryAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Tag")
	FGameplayTag EventTagToWaitFor;

	UPROPERTY(EditDefaultsOnly, Category = "Setup|SphereHitTrace")
	float SphereRadius = 120;
	UPROPERTY(EditDefaultsOnly, Category = "Setup|SphereHitTrace")
	float VerticalOffset = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setup|SphereHitTrace")
	float SphereCenterOffSet = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Setup|CollisionChannel")
	TEnumAsByte<ECollisionChannel> HitCollisionChannel = ECollisionChannel::ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, Category="Setup|VFX")
	TObjectPtr<UNiagaraSystem> PrimaryAttackVFX;
	UPROPERTY(EditDefaultsOnly, Category="Setup|VFX")
	TArray<FName> MontageSections;

	UPROPERTY(EditDefaultsOnly, Category="Setup|GameplayEffect")
	TSubclassOf<UGameplayEffect> PrimaryDamageGameplayEffect;

	
#pragma endregion
	
	TWeakObjectPtr<ACC_PlayerCharacter> CC_PlayerCharacter;

	void CleanUpAndEndAbility();
	void CC_CancelAbility();
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayMontageTask{nullptr};
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask{nullptr};
	UFUNCTION()
	void OnMontageFinished();
	UFUNCTION()
	void OnMontageCancelled();
	UFUNCTION()
	void OnMontageInterrupted();
	UFUNCTION()
	void OnMontageBlendOut();

	UFUNCTION()
	void OnGameplayEventReceived(FGameplayEventData Payload);

	void TraceHitSphere();

	FName GetRandomMontageSectionNameFromArray();
};
