// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/Player/CC_GA_ListenKillScored.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Tags/CC_GameplayTags.h"
#include "Utils/CC_GameplayStatics.h"

UCC_GA_ListenKillScored::UCC_GA_ListenKillScored():Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(CCTags::Ability::AutoActivate);
	SetAssetTags(Tags);
}

void UCC_GA_ListenKillScored::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CCTags::Event::Player::KillScored);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
	WaitGameplayEventTask->ReadyForActivation();
}

void UCC_GA_ListenKillScored::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCC_GA_ListenKillScored::OnEventReceived(FGameplayEventData Payload)
{
	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("Kill Scored: killed %s"), *Payload.Instigator.GetName());
		UCC_GameplayStatics::PrintScreenDebugMessage(*Msg);
	}
}
