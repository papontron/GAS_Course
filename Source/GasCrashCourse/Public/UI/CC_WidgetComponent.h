// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "CC_WidgetComponent.generated.h"


class UCC_AbilitySystemComponent;
class ACC_BaseCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASCRASHCOURSE_API UCC_WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UCC_WidgetComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	TWeakObjectPtr<ACC_BaseCharacter> OwnerCharacter;
	TWeakObjectPtr<UCC_AbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UCC_AttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere, Category="Setup|Attributes")
	TMap<FGameplayAttribute, FGameplayAttribute> AttributePairs;

	void InitAbilitySystemData();
	bool IsASCInitialized() const;
	void InitializeAttributeDelegate();
	void BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;

	UFUNCTION()
	void OnASCInitializedDelegate(UAbilitySystemComponent* ASC, UAttributeSet* AS);

	UFUNCTION()
	void BindToAttributeChanges();
};
