// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_MeelePrimary.generated.h"

class UNiagaraSystem;
class ACC_Enemycharacter;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_MeleePrimary : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:
	UCC_MeleePrimary();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
#pragma region Setup
	UPROPERTY(EditDefaultsOnly, Category="Setup|VFX")
	TObjectPtr<UNiagaraSystem> HitVFX;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Montage")
	TArray<UAnimMontage*> AttackMontages;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Collision")
	TEnumAsByte<ECollisionChannel> HitCollisionChannel;
	UPROPERTY(EditDefaultsOnly, Category="Setup|GameplayEffect")
	TSubclassOf<UGameplayEffect> DamageEffect;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Damage", meta=(ClampMax=0))
	float Damage = -15.f;
#pragma endregion
	TWeakObjectPtr<ACC_Enemycharacter> EnemyOwner;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayMontageTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask;
	void Attack();

	UFUNCTION()
	void OnCompleted();
	UFUNCTION()
	void OnCancelled();
	UFUNCTION()
	void OnInterrupted();
	UFUNCTION()
	void OnBlendOut();

	UFUNCTION()
	void OnEventReceived(FGameplayEventData Data);

	void RotateToTarget(AActor* Target);
	
};
