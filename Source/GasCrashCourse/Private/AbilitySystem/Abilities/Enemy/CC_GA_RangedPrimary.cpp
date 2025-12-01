// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/Enemy/CC_GA_RangedPrimary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/CC_Enemycharacter.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tags/CC_GameplayTags.h"
#include "Utils/CC_GameplayStatics.h"

UCC_GA_RangedPrimary::UCC_GA_RangedPrimary()
{
	//bRetriggerInstancedAbility = true;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	FGameplayTagContainer NewAssetTags = GetAssetTags();
	NewAssetTags.AddTag(CCTags::Ability::Enemy::Primary);
	// Tags.AddTag(CCTags::Ability::AutoActivate);
	SetAssetTags(NewAssetTags);
	FGameplayTagContainer NewSourceBlockedTags;
	NewSourceBlockedTags.AddTag(CCTags::Status::HitReacting);
	ActivationBlockedTags = NewSourceBlockedTags; 
}

void UCC_GA_RangedPrimary::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	EnemyOwner = Cast<ACC_Enemycharacter>(OwnerCharacter);
	//listen for gameplayevent "PrimaryAttack"
	// WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CCTags::Event::Enemy::PrimaryAttack);
	// WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
	// WaitGameplayEventTask->ReadyForActivation();
	Attack();
}

void UCC_GA_RangedPrimary::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (WaitGameplayEventTask && WaitGameplayEventTask->IsActive())
	{
		WaitGameplayEventTask->EndTask();
		WaitGameplayEventTask = nullptr;
	}
	if (EnemyOwner.IsValid() && EnemyOwner->RotationTimeline != nullptr && EnemyOwner->RotationTimeline->IsPlaying())
	{
		EnemyOwner->RotationTimeline->Stop();
		// EnemyOwner->RotationTimeline = nullptr;
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCC_GA_RangedPrimary::OnEventReceived(FGameplayEventData EventData)
{
	
	AActor* Instigator = const_cast<AActor*>(EventData.Instigator.Get());
	checkf(Instigator, TEXT("Failed to retrieve instigator in ranged primary attack ability"));
	TargetCharacter = Cast<ACC_PlayerCharacter>(Instigator);
	if (TargetCharacter.IsValid() && EnemyOwner.IsValid())
	{
		// //ratate towards target before attack; this starts the timeline
		// EnemyOwner->Server_RotateTowardsTarget(TargetCharacter.Get());
		// //Play attack montage automatically will trigger after timeline finishes playing
		EnemyOwner->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(EnemyOwner->GetActorLocation(), TargetCharacter->GetActorLocation()));
		//Spawn Projectile:
		SpawnProjectile();
		PlayAttackMontage();
	}
}



void UCC_GA_RangedPrimary::PlayAttackMontage()
{
	if (!IsValid(AttackMontage))
	{
		UCC_GameplayStatics::PrintScreenDebugMessage(FString("No attack montage was provided"));
		return;
	}
	
	// if (!EnemyOwner.IsValid() || !EnemyOwner->GetMesh() || !EnemyOwner->GetMesh()->GetAnimInstance()) return;
	// EnemyOwner->GetMesh()->GetAnimInstance()->StopAllMontages(true);
	// EnemyOwner->GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	
	PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage, 1.0f);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnBlendOut);
	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterrupted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnCancelled);
	PlayMontageTask->ReadyForActivation();
}

void UCC_GA_RangedPrimary::OnCompleted()
{
	 CleanUpAndEndAbility(false);
}

void UCC_GA_RangedPrimary::OnCancelled()
{
	CleanUpAndEndAbility(true);
}

void UCC_GA_RangedPrimary::OnInterrupted()
{
	 CleanUpAndEndAbility(false);
}

void UCC_GA_RangedPrimary::OnBlendOut()
{
	CleanUpAndEndAbility(false);
}

void UCC_GA_RangedPrimary::SpawnProjectile()
{
	FVector Location = GetOwningComponentFromActorInfo()->GetSocketLocation(SpawnProjectileSocketName);
	FRotator Rotation = GetOwningComponentFromActorInfo()->GetSocketRotation(SpawnProjectileSocketName);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(EnemyOwner.Get());
	GetWorld()->SpawnActor<AActor>(ProjectileClass, Location, Rotation,SpawnParams);
}

void UCC_GA_RangedPrimary::Attack()
{
	if (bDebug) UCC_GameplayStatics::PrintScreenDebugMessage(TEXT("Primary Attack event received"));
	
	AActor* Instigator = EnemyOwner->GetTargetActor();
	checkf(Instigator, TEXT("Failed to retrieve instigator in ranged primary attack ability"));
	TargetCharacter = Cast<ACC_PlayerCharacter>(Instigator);
	if (TargetCharacter.IsValid() && EnemyOwner.IsValid())
	{
		// //ratate towards target before attack; this starts the timeline
		// EnemyOwner->Server_RotateTowardsTarget(TargetCharacter.Get());
		// //Play attack montage automatically will trigger after timeline finishes playing
		EnemyOwner->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(EnemyOwner->GetActorLocation(), TargetCharacter->GetActorLocation()));
		//Spawn Projectile:
		SpawnProjectile();
		PlayAttackMontage();
	}
}
