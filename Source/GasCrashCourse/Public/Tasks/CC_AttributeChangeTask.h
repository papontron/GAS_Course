// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AttributeSet.h"
#include "CC_AttributeChangeTask.generated.h"

struct FOnAttributeChangeData;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);
UCLASS(BlueprintType, meta=(ExposedAsyncProxy=AsyncTaskInstance))
class GASCRASHCOURSE_API UCC_AttributeChangeTask : public UBlueprintAsyncActionBase
{																																
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly=true))
	static UCC_AttributeChangeTask* ListenForAttributeChange(UAbilitySystemComponent* ASC, FGameplayAttribute Attribute);
	
	void OnAttributeChangedCallback(const FOnAttributeChangeData& ChangedData);

	UFUNCTION(BlueprintCallable)
	void EndTask();

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChangedDelegate;
	
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	FGameplayAttribute AttributeToListenFor;
};
