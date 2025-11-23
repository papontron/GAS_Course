// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "Tags/CC_GameplayTags.h"
void UCC_AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	HandleAutoActivateAbility(AbilitySpec);
}

void UCC_AbilitySystemComponent::IncreaseAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Quantity)
{
	if (IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass);
	if (!AbilitySpec)
	{
		const TCHAR* Message = *FString::Printf(TEXT("Ability Spect Not Found!"));
		GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Black, Message);
		return;
	}
	if (AbilitySpec)
	{
		AbilitySpec->Level += Quantity;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}



void UCC_AbilitySystemComponent::SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 AbilityLevel)
{
	//level up abilities should only be made in the server
	if (IsValid(GetAvatarActor() )&& !GetAvatarActor()->HasAuthority()) return;
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass);
	if (!AbilitySpec)
	{
		const TCHAR* Message = *FString::Printf(TEXT("Ability Spec Not Found!"));
		GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Black, Message);
		return;
	}
	AbilitySpec->Level = AbilityLevel;
	MarkAbilitySpecDirty(*AbilitySpec);
}

void UCC_AbilitySystemComponent::HandleAutoActivateAbility(const FGameplayAbilitySpec& AbilitySPec)
{
	for (const FGameplayTag& Tag :AbilitySPec.Ability->GetAssetTags())
	{
		if (Tag.MatchesTagExact(CCTags::Ability::AutoActivate))
		{
			TryActivateAbility(AbilitySPec.Handle);
			return;
		}
	}
}
