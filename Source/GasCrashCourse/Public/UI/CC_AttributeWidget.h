// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "CC_AttributeWidget.generated.h"

/**
 * 
 */
using FAttributePair = TTuple<FGameplayAttribute, FGameplayAttribute> ;
UCLASS()
class GASCRASHCOURSE_API UCC_AttributeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category="AvatarActor")
	TWeakObjectPtr<AActor> AvatarActor;
	
	void OnAttributeChange(const FAttributePair & Pair,const UCC_AttributeSet* AttributeSet, float OldValue);
	bool MatchesAttributes(const FAttributePair & Pair);
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Attribute Changed"))
	void BP_OnAttributeChanged(float AttributeValue, float MaxAttributeValue, float OldValue);
private:
	UPROPERTY(EditAnywhere, Category="Setup|Attributes")
	FGameplayAttribute Attribute;
	UPROPERTY(EditAnywhere, Category="Setup|Attributes")
	FGameplayAttribute MaxAttribute;
	
};
