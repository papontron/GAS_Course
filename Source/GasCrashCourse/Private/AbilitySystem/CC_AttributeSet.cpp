// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/CC_AttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Characters/CC_BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Tags/CC_GameplayTags.h"

void UCC_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCC_AttributeSet, bAttributesInitialized, COND_None, REPNOTIFY_Always)
}

void UCC_AttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	AvatarActor = Cast<ACC_BaseCharacter>(GetActorInfo()->AvatarActor.Get());
	if (Data.EvaluatedData.Attribute ==GetHealthAttribute())	//requires "GameplayEffectExtension.h"
	{
		const float NewHealth = FMath::Clamp(GetHealth(),0,GetMaxHealth());
		
		if (NewHealth != GetHealth())
		{
			if (NewHealth ==0)
			{
				SendKillScoredEventToInstigator(Data);
			}
			SetHealth(NewHealth);
		}
		
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		const float NewMana =FMath::Clamp(GetMana(),0,GetMaxMana());
		if (NewMana != GetMana()) SetMana(NewMana);
		
	}
		
	if (!bAttributesInitialized)
	{
		bAttributesInitialized = true;
		OnAttributesInitialized.Broadcast();
	}
}

void UCC_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health,OldValue)
}

void UCC_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth,OldValue)
}

void UCC_AttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue)
}

void UCC_AttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue)
}

void UCC_AttributeSet::OnRep_AttributesInitialized()
{
	//for the client
	if (bAttributesInitialized)
	{
		OnAttributesInitialized.Broadcast();
	}
}

void UCC_AttributeSet::SendKillScoredEventToInstigator(const FGameplayEffectModCallbackData& Data)
{
	if (!AvatarActor.IsValid()) return;
	if (!AvatarActor.Get()->IsAlive()) return;
	AActor* Instigator = Data.EffectSpec.GetContext().GetInstigator(); //Data.Target.GetAvatarActor();
	FGameplayEventData Payload;
	Payload.Instigator = AvatarActor.Get();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor( Instigator,CCTags::Event::Player::KillScored, Payload);
}
