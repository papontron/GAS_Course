// Papontron Inc. All Rigths reserved.


#include "Player/CC_PlayerController.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Tags/CC_GameplayTags.h"

void ACC_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(Subsystem, TEXT("ERROR RETRIEVING SUBSYSTEM"));
	for (auto & MappingContext : InputMappingContexts)
	{
		Subsystem->AddMappingContext(MappingContext,0);
	}
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInputComponent, TEXT("Failed to cast EnhancedInputComponent"));
	if (MoveInputAction)
	{
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	}
	if (LookInputAction)
	{
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	}
	if (JumpInputAction)
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this,&ThisClass::Jump);
	}
	if (PrimaryAttackInputAction)
	{
		EnhancedInputComponent->BindAction(PrimaryAttackInputAction,ETriggerEvent::Started, this, &ThisClass::PrimaryAttack);
	}
	if (LevelUpPrimaryAbilityInputAction)
	{
		EnhancedInputComponent->BindAction(LevelUpPrimaryAbilityInputAction, ETriggerEvent::Started, this, &ThisClass::LevelUpPrimaryAbility);
	}
}

void ACC_PlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(GetCharacter())) return;
	const FVector2D MoveVector = Value.Get<FVector2D>();
	const FRotator YawRotator = FRotator(0.f, GetControlRotation().Yaw, 0.0f);
	const FVector ForwardVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);
	GetCharacter()->AddMovementInput(ForwardVector, MoveVector.Y);
	GetCharacter()->AddMovementInput(RightVector, MoveVector.X);
}

void ACC_PlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y);
}

void ACC_PlayerController::Jump()
{
	if (!GetCharacter()) return;
	GetCharacter()->Jump();
}

void ACC_PlayerController::StopJump()
{
	GetCharacter()->StopJumping();
}

void ACC_PlayerController::PrimaryAttack()
{
	ActivateAbility(CCTags::Ability::Player::Primary);
}

void ACC_PlayerController::LevelUpPrimaryAbility_Implementation()
{
	if (UCC_AbilitySystemComponent* ASC = Cast<UCC_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetCharacter())))
	{
		auto AbilityClass = Cast<ACC_BaseCharacter>(GetCharacter())->GetPrimaryAttackAbilityClass();
		ASC->SetAbilityLevel(AbilityClass, 10);
	}
}

void ACC_PlayerController::ActivateAbility(const FGameplayTag& Tag)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!ASC) return;
	ASC->TryActivateAbilitiesByTag(Tag.GetSingleTagContainer());
}


