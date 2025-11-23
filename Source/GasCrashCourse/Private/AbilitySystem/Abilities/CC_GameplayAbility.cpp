// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "Tags/CC_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Characters/CC_BaseCharacter.h"

UCC_GameplayAbility::UCC_GameplayAbility()
{
	FGameplayTagContainer BlockedTags;
	BlockedTags.AddTag(CCTags::Status::Dead);
	ActivationBlockedTags = BlockedTags;
}

void UCC_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!OwnerCharacter.IsValid())
	{
		OwnerCharacter = Cast<ACC_BaseCharacter>(GetAvatarActorFromActorInfo());
	}
}

void UCC_GameplayAbility::CC_ApplyGameplayEffectToTarget(const TSubclassOf<UGameplayEffect>& EffectClass,
	UAbilitySystemComponent* TargetASC) const
{
	if (GetAbilitySystemComponentFromActorInfo())
	{
		const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		const int32 AbilityLevel = GetAbilityLevel();
		const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass, AbilityLevel, ContextHandle);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UCC_GameplayAbility::CC_ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& EffectClass) const
{
	if (!GetAbilitySystemComponentFromActorInfo()) return;
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	const int32 AbilityLevel = GetAbilityLevel();
	GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Blue, *FString::Printf(TEXT("AbilityLevel: %d"), AbilityLevel));
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass, AbilityLevel, ContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
