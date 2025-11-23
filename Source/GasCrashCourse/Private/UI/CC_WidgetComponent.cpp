// Papontron Inc. All Rigths reserved.


#include "UI/CC_WidgetComponent.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/CC_BaseCharacter.h"
#include "UI/CC_AttributeWidget.h"


UCC_WidgetComponent::UCC_WidgetComponent()
{
	Space = EWidgetSpace::Screen;
}

// Called when the game starts
void UCC_WidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	//Init the data of this widget component
	InitAbilitySystemData();
	//check if the data is correctly initialized
	if (!IsASCInitialized()){
		//if ASC and Attribute set are not valid yet, set a delegate
		OwnerCharacter->OnASCInitialized.AddDynamic(this, &ThisClass::OnASCInitializedDelegate);
		return;
	}
	//initialize to attributes and bind to attribute change delegates
	InitializeAttributeDelegate();
}

void UCC_WidgetComponent::InitAbilitySystemData()
{
	OwnerCharacter = Cast<ACC_BaseCharacter>(GetOwner());
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(OwnerCharacter->GetAbilitySystemComponent());
	AttributeSet =Cast<UCC_AttributeSet>(OwnerCharacter->GetAttributeSet());
}

bool UCC_WidgetComponent::IsASCInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UCC_WidgetComponent::InitializeAttributeDelegate()
{
	if (AttributeSet->bAttributesInitialized)//check if the attribute set has been initialized
	{
		BindToAttributeChanges();
		return;
	}
	AttributeSet->OnAttributesInitialized.AddDynamic(this,&ThisClass::BindToAttributeChanges);
}

void UCC_WidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject,
	const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	UCC_AttributeWidget* AttributeWidget = Cast<UCC_AttributeWidget>(WidgetObject);
	if (!IsValid(AttributeWidget)) return;
	if (!AttributeWidget->MatchesAttributes(Pair)) return;
	AttributeWidget->AvatarActor = OwnerCharacter;
	AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), 0.f); //for initial updates
	//for future updates:
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key).AddLambda([&Pair, this, AttributeWidget](const FOnAttributeChangeData& AttributeChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), AttributeChangeData.OldValue);
	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddLambda([&Pair, this, AttributeWidget](const FOnAttributeChangeData& AttributeChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), AttributeChangeData.OldValue);
	});
}

void UCC_WidgetComponent::OnASCInitializedDelegate(UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(ASC);
	AttributeSet = Cast<UCC_AttributeSet>(AS);
	
	if (!IsASCInitialized()) return; //if ASC and AS are still invalid, return
	InitializeAttributeDelegate(); //once they are valid, initialize the attributes
}

void UCC_WidgetComponent::BindToAttributeChanges() //trigger after the attributes from the attribute set are initialized
{
	//Bind all widgets to their corresponding attributes
	for (const auto & Pair: AttributePairs)
	{
		if (UCC_AttributeWidget* AttributeWidget = Cast<UCC_AttributeWidget>(GetUserWidgetObject())) //check if the owned widget is an AttributeWidget
		{
			BindWidgetToAttributeChanges(AttributeWidget, Pair);
		}else
		{
			GetUserWidgetObject()->WidgetTree->ForEachWidget([&Pair,this](UWidget* ChildWidget)
			{	
				BindWidgetToAttributeChanges(ChildWidget, Pair);
			});
		}
	}
}



