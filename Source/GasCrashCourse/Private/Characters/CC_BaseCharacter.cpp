// Papontron Inc. All Rigths reserved.


#include "GasCrashCourse/Public/Characters/CC_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Tags/CC_GameplayTags.h"
#include "Utils/CC_GameplayStatics.h"


// Sets default values
ACC_BaseCharacter::ACC_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	// Tick and refresh bone transforms whether rendered or not - for bone updates on a dedicated server
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}
void ACC_BaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bIsAlive);
}

UAbilitySystemComponent* ACC_BaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

UAttributeSet* ACC_BaseCharacter::GetAttributeSet() const
{
	return nullptr;
}


void ACC_BaseCharacter::GiveStartupAbilities()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	checkf(ASC,TEXT("Failed to retrieve ASC when giving startup abilities"))
	//PrimaryAttack
	if (IsValid(PrimaryAttackAbilityClass))
	{
		ASC->GiveAbility(FGameplayAbilitySpec(PrimaryAttackAbilityClass));
	}
	//Death Ability
	if (IsValid(DeathAbility))
	{
		ASC->GiveAbility(FGameplayAbilitySpec(DeathAbility));
	}
	for (const auto & Ability : StartupAbilities)
	{
		if (IsValid(Ability))
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

void ACC_BaseCharacter::InitializeAttributes()
{
	if (!IsValid(InitializeAttributesEffect)) return;
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	checkf(ASC,TEXT("Failed to retrieve ASC when initializing attributes"));
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InitializeAttributesEffect,1,ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ACC_BaseCharacter::RestartAttributes()
{
	if (GetAbilitySystemComponent()&&RestartAttributesEffect)
	{
		FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
		ContextHandle.AddInstigator(this, this);
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(RestartAttributesEffect, 1, ContextHandle);
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ACC_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACC_BaseCharacter::RestartAbilities()
{
	if (GetAbilitySystemComponent())
	{
		for (auto& Spec:GetAbilitySystemComponent()->GetActivatableAbilities())
		{
			if (Spec.Ability->GetAssetTags().HasTagExact(CCTags::Ability::AutoActivate))
			{
				GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void ACC_BaseCharacter::OnHealthChangedCallBack(const FOnAttributeChangeData& ChangedData)
{
	if (ChangedData.NewValue <= 0 && ChangedData.OldValue > 0 && bIsAlive)
	{
		GetCharacterMovement()->DisableMovement();
		// if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		// {
		// 	PlayerController->DisableInput(PlayerController);
		// }
		HandleDeath(); //activate death ability
	}
}

void ACC_BaseCharacter::HandleRespawn()
{
	bIsAlive = true;
	
	if (GetAbilitySystemComponent())
	{
		//CancelDeathAbility
		FGameplayTagContainer TagsToCancel;
		TagsToCancel.AddTag(CCTags::Ability::Death);
		GetAbilitySystemComponent()->CancelAbilities(&TagsToCancel);
		//remove the death tag from ASC;
		GetAbilitySystemComponent()->RemoveLooseGameplayTag(CCTags::Status::Dead);
	}
	
	//Restart attributes;
	RestartAttributes();
	//Restart Abilities
	RestartAbilities();
	//enable movement
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ACC_BaseCharacter::HandleDeath()
{
	bIsAlive = false;
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC) || !IsValid(DeathAbility)) return;
	UCC_GameplayStatics::PrintScreenDebugMessage(FString::Printf(TEXT("%s has died"),*GetName()));
	for (auto & Spec: GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (!IsValid(Spec.Ability)) continue;
		if (Spec.Ability->GetAssetTags().HasTagExact(CCTags::Ability::Death))
		{
			ASC->TryActivateAbility(Spec.Handle);
			return;
		}
	}
}




