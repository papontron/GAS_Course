// Papontron Inc. All Rigths reserved.


#include "Player/CC_PlayerState.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"

ACC_PlayerState::ACC_PlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UCC_AbilitySystemComponent>("PlayerAbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UCC_AttributeSet>("PlayerAttributeSet");
}

UAbilitySystemComponent* ACC_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ACC_PlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
