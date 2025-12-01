// Papontron Inc. All Rigths reserved.


#include "AbilitySystem/Abilities/Enemy/CC_MeelePrimary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/CC_BaseCharacter.h"
#include "Characters/CC_Enemycharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/CC_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utils/CC_GameplayStatics.h"

UCC_MeleePrimary::UCC_MeleePrimary()
{
	// bRetriggerInstancedAbility = true;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	FGameplayTagContainer NewAssetTags = GetAssetTags();
	NewAssetTags.AddTag(CCTags::Ability::Enemy::Primary);
	SetAssetTags(NewAssetTags);
	//TODO add tags that will cancel this ability
	FGameplayTagContainer BlockingTags;
	BlockingTags.AddTag(CCTags::Status::HitReacting);
	ActivationBlockedTags = BlockingTags; 
}

void UCC_MeleePrimary::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	EnemyOwner = Cast<ACC_Enemycharacter>(OwnerCharacter);
	//the Attack Montages are expected to send an event notify to make a sphere trace
	Attack();
	WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CCTags::Event::Enemy::PrimaryAttack);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &UCC_MeleePrimary::OnEventReceived);
	WaitGameplayEventTask->ReadyForActivation();
}

void UCC_MeleePrimary::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCC_MeleePrimary::Attack()
{
	if (!EnemyOwner.IsValid() || !EnemyOwner->IsAlive()) return;
	if (AttackMontages.Num()==0) return;
	if (PlayMontageTask && PlayMontageTask->IsActive()) return;
	RotateToTarget(EnemyOwner->GetTargetActor());
	int RandomIndex = UCC_GameplayStatics::GetRandomArrayIndex(AttackMontages.Num());
	PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontages[RandomIndex]);
	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnCancelled);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterrupted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnBlendOut);
	PlayMontageTask->ReadyForActivation();
}

void UCC_MeleePrimary::OnCompleted()
{
	CleanUpAndEndAbility(false);
}

void UCC_MeleePrimary::OnCancelled()
{
	CleanUpAndEndAbility(true);
}

void UCC_MeleePrimary::OnInterrupted()
{
	CleanUpAndEndAbility(false);
}

void UCC_MeleePrimary::OnBlendOut()
{
	CleanUpAndEndAbility(false);
}

void UCC_MeleePrimary::OnEventReceived(FGameplayEventData Data)
{
	//Trace Sphere Hit
	if (!EnemyOwner.IsValid()) return;
	
	const FVector Start = EnemyOwner->GetActorLocation()+EnemyOwner->GetActorForwardVector()*EnemyOwner->GetAttackRange()+FVector(0,0,50.f);
	const float Radius = EnemyOwner->GetAttackRange();
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(EnemyOwner.Get());
	TArray<FHitResult> HitResults;
	EDrawDebugTrace::Type DebugTraceType = bDebug? EDrawDebugTrace::ForDuration:EDrawDebugTrace::None;
	UKismetSystemLibrary::SphereTraceMulti(OwnerCharacter.Get(),Start,Start,Radius,
		UEngineTypes::ConvertToTraceType(HitCollisionChannel), false,IgnoreActors,DebugTraceType,HitResults,true);
	//Validate each Hit result
	for (const FHitResult& HitResult : HitResults)
	{
		//Send HitReactEvent & apply damage
		if (!IsValid(HitResult.GetActor())) continue;
		ACC_PlayerCharacter* PlayerHit = Cast<ACC_PlayerCharacter>(HitResult.GetActor());
		if (!IsValid(PlayerHit)) continue;
		UAbilitySystemComponent* TargetASC = PlayerHit->GetAbilitySystemComponent();
		if (!IsValid(TargetASC)) continue;
		FGameplayEventData Payload;
		Payload.Instigator = EnemyOwner.Get();
		FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
		ContextHandle.AddHitResult(HitResult);
		ContextHandle.AddInstigator(EnemyOwner.Get(),EnemyOwner.Get());
		Payload.ContextHandle =ContextHandle;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerHit,CCTags::Event::HitReact, Payload);
		FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect,GetAbilityLevel(),ContextHandle);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, CCTags::SetByCaller::MeleeAttack, Damage); 
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (IsValid(HitVFX)){
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(EnemyOwner.Get(), HitVFX, HitResult.ImpactPoint);
		}
		
	}
}

void UCC_MeleePrimary::RotateToTarget(AActor* Target)
{
	if (!IsValid(Target) || !EnemyOwner.IsValid()) return;
	EnemyOwner->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(EnemyOwner->GetActorLocation(), Target->GetActorLocation()));
}
