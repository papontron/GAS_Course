// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CC_AttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class ACC_BaseCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttributesInitialized);

UCLASS()
class GASCRASHCOURSE_API UCC_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCC_AttributeSet, Health)
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCC_AttributeSet, MaxHealth)
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UCC_AttributeSet, Mana)
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UCC_AttributeSet, MaxMana)
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
	
#pragma region AttributesInitialized
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttributesInitialized)
	bool bAttributesInitialized = false;
	UFUNCTION()
	void OnRep_AttributesInitialized();
	/**
	 * 
	 */
	UPROPERTY(BlueprintAssignable)
	FAttributesInitialized OnAttributesInitialized;
#pragma endregion
	void SendKillScoredEventToInstigator(const FGameplayEffectModCallbackData& Data);

	TWeakObjectPtr<ACC_BaseCharacter> AvatarActor;
};





