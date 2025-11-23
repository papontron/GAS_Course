// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "CC_BaseCharacter.generated.h"

struct FOnAttributeChangeData;
class UCC_GameplayAbility;
class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FASCInitialized, UAbilitySystemComponent*, ASC, UAttributeSet* , AS);

UCLASS()
class GASCRASHCOURSE_API ACC_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	ACC_BaseCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const;
	
	TSubclassOf<UGameplayAbility> GetPrimaryAttackAbilityClass() const{ return PrimaryAttackAbilityClass;}
	
	UPROPERTY(BlueprintAssignable)
	FASCInitialized OnASCInitialized;

	void SetIsAlive(bool IsAliveInput) { bIsAlive = IsAliveInput; }
	UFUNCTION(BlueprintCallable, Category="BaseCharacter")
	bool IsAlive()const { return bIsAlive; }

	UFUNCTION(BlueprintCallable, Category="CC|Death")
	virtual void HandleRespawn();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	
protected:
	void GiveStartupAbilities();
	void InitializeAttributes();
	void RestartAttributes();
	virtual void BeginPlay() override;
	void RestartAbilities();
#pragma region Setup
	UPROPERTY(EditDefaultsOnly, Category="Setup|GameplayAbility")
	TArray<TSubclassOf<UCC_GameplayAbility>> StartupAbilities;
	UPROPERTY(EditDefaultsOnly, Category="Setup|GameplayAbility")
	TSubclassOf<UGameplayAbility> PrimaryAttackAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category="Setup|GameplayAbility")
	TSubclassOf<UGameplayAbility> DeathAbility;
	UPROPERTY(EditDefaultsOnly, Category="Setup|GameplayEffect")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	UPROPERTY(EditDEfaultsOnly, Category="Setup|GameplayEffect")
	TSubclassOf<UGameplayEffect> RestartAttributesEffect;
	
#pragma endregion
	void OnHealthChangedCallBack(const FOnAttributeChangeData& ChangedData);
	virtual void HandleDeath();
	
private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	bool bIsAlive = true;
};
