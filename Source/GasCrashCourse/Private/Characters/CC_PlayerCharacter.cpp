// Papontron Inc. All Rigths reserved.


#include "GasCrashCourse/Public/Characters/CC_PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/CC_PlayerState.h"

namespace PlayerTag
{
	FName PlayerTag = "Player";
}

// Sets default values
ACC_PlayerCharacter::ACC_PlayerCharacter()
{
	
	PrimaryActorTick.bCanEverTick = false;
	//Only the spring arm will follow the control rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f,540.f,0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);
	CameraComponent->bUsePawnControlRotation = false;
	//add tag
	Tags.Add(PlayerTag::PlayerTag);
}

UAbilitySystemComponent* ACC_PlayerCharacter::GetAbilitySystemComponent() const
{
	
	if (const ACC_PlayerState* CC_PlayerState = Cast<ACC_PlayerState>(GetPlayerState())) return CC_PlayerState->GetAbilitySystemComponent();
	
	return nullptr;
}

UAttributeSet* ACC_PlayerCharacter::GetAttributeSet() const
{
	if (const ACC_PlayerState* CC_PlayerState = Cast<ACC_PlayerState>(GetPlayerState())) return CC_PlayerState->GetAttributeSet();
	return nullptr;
		
}

// Called when the game starts or when spawned
void ACC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACC_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!IsValid(GetAbilitySystemComponent())||!HasAuthority()) return;
	
	InitializeAbilitySystemActorInfo();
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
	GiveStartupAbilities();
	InitializeAttributes();
	UCC_AttributeSet* AS = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(AS)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChangedCallBack);
}

void ACC_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (!IsValid(GetAbilitySystemComponent())) return;
	InitializeAbilitySystemActorInfo();
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
	UCC_AttributeSet* AS = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(AS)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChangedCallBack);
}

void ACC_PlayerCharacter::InitializeAbilitySystemActorInfo()
{
	ACC_PlayerState* CC_PlayerState = Cast<ACC_PlayerState>(GetPlayerState());
	checkf(CC_PlayerState, TEXT("Unable to retrieve player state"))
	CC_PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(CC_PlayerState, this);
}



