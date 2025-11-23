// Papontron Inc. All Rigths reserved.


#include "Tasks/CC_AttributeChangeTask.h"
#include "AbilitySystemComponent.h"
UCC_AttributeChangeTask* UCC_AttributeChangeTask::ListenForAttributeChange(UAbilitySystemComponent* ASC,
	FGameplayAttribute Attribute)
{
	if (!IsValid(ASC)) return nullptr;
	UCC_AttributeChangeTask* WaitForAttributeChangeTask = NewObject<UCC_AttributeChangeTask>();
	// if (!IsValid(ASC)) WaitForAttributeChangeTask->RemoveFromRoot();
	WaitForAttributeChangeTask->AbilitySystemComponent= ASC;
	WaitForAttributeChangeTask->AttributeToListenFor = Attribute;
	ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangeTask, &UCC_AttributeChangeTask::OnAttributeChangedCallback);
	return WaitForAttributeChangeTask;
}

void UCC_AttributeChangeTask::OnAttributeChangedCallback(const FOnAttributeChangeData& ChangedData)
{
	OnAttributeChangedDelegate.Broadcast(ChangedData.Attribute, ChangedData.NewValue, ChangedData.OldValue);
}

void UCC_AttributeChangeTask::EndTask()
{
	if (AbilitySystemComponent.IsValid())
	{
		//remove all callbacks that this class has bound to this delegate
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);
	}
	SetReadyToDestroy();
	MarkAsGarbage();
}
