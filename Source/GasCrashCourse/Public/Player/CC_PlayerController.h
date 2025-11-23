// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CC_PlayerController.generated.h"
struct FGameplayTag;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
UCLASS()
class GASCRASHCOURSE_API ACC_PlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void SetupInputComponent() override;
private:
#pragma region Input
	UPROPERTY(EditDefaultsOnly, Category="Setup|Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Input")
	TObjectPtr<UInputAction> MoveInputAction;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Input")
	TObjectPtr<UInputAction> JumpInputAction;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Input")
	TObjectPtr<UInputAction> LookInputAction;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Input")
	TObjectPtr<UInputAction> PrimaryAttackInputAction;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Input")
	TObjectPtr<UInputAction> LevelUpPrimaryAbilityInputAction;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump();
	void StopJump();
	void PrimaryAttack();
	UFUNCTION(Server,Reliable)
	void LevelUpPrimaryAbility();
#pragma endregion Input
	void ActivateAbility(const FGameplayTag& Tag);
	
	
};
