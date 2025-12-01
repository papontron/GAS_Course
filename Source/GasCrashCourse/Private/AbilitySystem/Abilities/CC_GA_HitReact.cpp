// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/CC_GA_HitReact.h"

#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/CC_BaseCharacter.h"
#include "Tags/CC_GameplayTags.h"

UCC_GA_HitReact::UCC_GA_HitReact():Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	FGameplayTag AbilityTagToCancel = CCTags::Ability::Enemy::Primary;
	CancelAbilitiesWithTag = AbilityTagToCancel.GetSingleTagContainer();
	FGameplayTagContainer NewAssetTags = GetAssetTags();
	NewAssetTags.AddTag( CCTags::Ability::HitReact);
	SetAssetTags(NewAssetTags);
	
}

void UCC_GA_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//Only trigger once should be false in order to retrigger this ability
	WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CCTags::Event::HitReact);
	// WaitGameplayEventTask->OnlyTriggerOnce = true;
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &UCC_GA_HitReact::OnEventReceived);
	WaitGameplayEventTask->ReadyForActivation();
	
}

void UCC_GA_HitReact::CC_CancelAbility()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(CCTags::Status::HitReacting);
	}
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UCC_GA_HitReact::OnEventReceived(FGameplayEventData Payload)
{
	
	// if (HitReactVFX && IsValid(OwnerCharacter.Get()))
	// {
	// 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitReactVFX, OwnerCharacter.Get()->GetActorLocation());
	// }
	if (!IsValid(OwnerCharacter->GetMesh()->GetAnimInstance()))
	{
		UE_LOG(LogTemp,Warning, TEXT("Unable to retrieve anim instance"))
		return;
	}
	// if (!IsValid(HitReactMontage))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("HitReactMontage was not provided"))
	// 	return;
	// }
	
	
	// if (MontageTask && MontageTask->IsActive())
	// {
	// 	return;
	// }
	// if (ChanceToHitReact>FMath::FRandRange(0.f,1.f) )
	// {
		auto InputInstigator = ConstCast(Payload.Instigator);
		// const EHitDirection HitDirection = UCC_GameplayStatics::GetHitDirectionFromInstigator(GetAvatarActorFromActorInfo(),InputInstigator , true);
		// if (bDebug)
		// {
		// 	FString Msg = FString::Printf(TEXT("Direction: %s") ,*StaticEnum<EHitDirection>()->GetNameByValue(static_cast<int64>(HitDirection)).ToString());
		// 	UCC_GameplayStatics::PrintScreenDebugMessage(Msg);
		// }
		// const FName MontageSection = GetMontageSectionName(HitDirection);
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(CCTags::Status::HitReacting);
		//Play Montage
		// MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HitReactMontage,1.f,MontageSection,true);
		// MontageTask->OnBlendOut.AddDynamic(this, &UCC_GA_HitReact::OnBlendOut);
		// MontageTask->OnInterrupted.AddDynamic(this, &UCC_GA_HitReact::OnInterrupted);
		// MontageTask->OnCompleted.AddDynamic(this, &UCC_GA_HitReact::OnCompleted);
		// MontageTask->OnCancelled.AddDynamic(this, &UCC_GA_HitReact::OnCancelled);
		// MontageTask->ReadyForActivation();
		FGameplayCueParameters Params;
		Params.AbilityLevel = GetAbilityLevel();
		Params.EffectContext = Payload.ContextHandle;
		Params.SourceObject = HitReactMontage;
		Params.Instigator = InputInstigator;
		// K2_ExecuteGameplayCueWithParams(CCTags::GameplayCue::Player::HitReact,Params);
	// }	
	
}

FName UCC_GA_HitReact::GetMontageSectionName(EHitDirection HitDirection)const{
	switch (HitDirection)
	{
		case EHitDirection::Back: return FName("Back");
		case EHitDirection::Left: return FName("Left");
		case EHitDirection::Right: return FName("Right");
		case EHitDirection::Front: return FName("Front");
		default: return NAME_None;
	}
}

void UCC_GA_HitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	if (MontageTask && MontageTask->IsActive())
	{
		MontageTask->EndTask();
	}
	if (WaitGameplayEventTask && WaitGameplayEventTask->IsActive())
	{
		WaitGameplayEventTask->EndTask();
	}
	MontageTask = nullptr;
	WaitGameplayEventTask = nullptr;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(CCTags::Status::HitReacting);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}
void UCC_GA_HitReact::OnBlendOut()
{
	SetRemoveTagTimer();
}

void UCC_GA_HitReact::OnInterrupted()
{
	SetRemoveTagTimer();
}

void UCC_GA_HitReact::OnCancelled()
{
	SetRemoveTagTimer();
}

void UCC_GA_HitReact::OnCompleted()
{
	SetRemoveTagTimer();
}

void UCC_GA_HitReact::RemoveHitReactingTag()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(CCTags::Status::HitReacting);
	}
}

void UCC_GA_HitReact::SetRemoveTagTimer()
{
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().ClearTimer(RemoveTagTimerHandle);
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]()
	{
		RemoveHitReactingTag();
	});
	GetWorld()->GetTimerManager().SetTimer(RemoveTagTimerHandle,TimerDelegate,DazeDuration, false, DazeDuration);
}
