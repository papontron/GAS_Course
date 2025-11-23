// Papontron Inc. All Rigths reserved.


#include "UI/CC_AttributeWidget.h"


void UCC_AttributeWidget::OnAttributeChange(const FAttributePair& Pair,const UCC_AttributeSet* AttributeSet, float OldValue)
{
	const float AttributeValue = Pair.Key.GetNumericValue(AttributeSet);
	const float MaxAttributeValue = Pair.Value.GetNumericValue(AttributeSet);
	BP_OnAttributeChanged(AttributeValue, MaxAttributeValue, OldValue);
}

bool UCC_AttributeWidget::MatchesAttributes(const FAttributePair& Pair)
{
	return Pair.Key == Attribute && Pair.Value == MaxAttribute;
}
