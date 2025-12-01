// Papontron Inc. All Rigths reserved.


#include "GasCrashCourse/Public/Characters/CC_Enemycharacter.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/CC_EnemyController.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Utils/CC_GameplayStatics.h"

// Sets default values
ACC_Enemycharacter::ACC_Enemycharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	AbilitySystemComponent = CreateDefaultSubobject<UCC_AbilitySystemComponent>("EnemyAbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UCC_AttributeSet>("EnemyAttributeSet");
	AIControllerClass = ACC_EnemyController::StaticClass();
	//Rotation
	
	RotationTimeline = CreateDefaultSubobject<UTimelineComponent>("RotationTimeline");
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFinder(TEXT("/Game/Curves/RotationCurve.RotationCurve"));
	if (CurveFinder.Succeeded())
	{
		RotationCurve = CurveFinder.Object;
	}
	// ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFinder(TEXT("/Game/Curves/RotationCurve.RotationCurve"));
	// if (CurveFinder.Succeeded())
	// {
	// 	//Bind the curve to the timeline
	// 	RotationCurve = CurveFinder.Object;
	// 	//on float update
	// 	FOnTimelineFloat TimelineProgress;
	// 	TimelineProgress.BindUFunction(this, FName("OnUpdateTimeline"));
	// 	RotationTimeline->AddInterpFloat(RotationCurve, TimelineProgress);
	// 	FOnTimelineEvent TimelineEvent;
	// 	TimelineEvent.BindUFunction(this, FName("OnFinishedTimeline"));
	// 	RotationTimeline->SetTimelineFinishedFunc(TimelineEvent);
	// }
}

UAbilitySystemComponent* ACC_Enemycharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}






UAttributeSet* ACC_Enemycharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ACC_Enemycharacter::SetTargetActor(AActor* NewTargetActor)
{
	if (TargetActor.Get() != NewTargetActor)
	{
		TargetActor = NewTargetActor;
	}
}

AActor* ACC_Enemycharacter::GetTargetActor() const
{
	return TargetActor.Get();
}

// Called when the game starts or when spawned
void ACC_Enemycharacter::BeginPlay()
{
	Super::BeginPlay();
	SetupRotationTimeline();
	if (!IsValid(AbilitySystemComponent)) return;
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	OnASCInitialized.Broadcast(AbilitySystemComponent,AttributeSet);
	if (!HasAuthority()) return;
	GiveStartupAbilities();
	InitializeAttributes();
	UCC_AttributeSet* AS = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(AS)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChangedCallBack);
}

void ACC_Enemycharacter::Destroyed()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	}
	Super::Destroyed();
}

void ACC_Enemycharacter::HandleRespawn()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	}
	Super::HandleRespawn();
	
	//Move to the new location
	AActor* RandomPlayerStart = UGameplayStatics::GetGameMode(this)->FindPlayerStart(GetController());
	SetActorLocation(RandomPlayerStart->GetActorLocation());
	
}

void ACC_Enemycharacter::HandleDeath()
{
	Super::HandleDeath();
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this]()
	{
		HandleRespawn();
	});
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle,TimerDel,RespawnTimeSeconds,false);
}



void ACC_Enemycharacter::OnUpdateTimeline(float Alpha)
{
	UCC_GameplayStatics::PrintScreenDebugMessage(FString("Playing timeline"));
	if (TargetActor.IsValid()) return;
	FRotator StartRotator = GetActorRotation();
	FRotator TargetRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation());
	FQuat TargetQuat = FQuat::Slerp(StartRotator.Quaternion(), TargetRotator.Quaternion(), Alpha);
	SetActorRotation(TargetQuat.Rotator());
}

void ACC_Enemycharacter::OnFinishedTimeline()
{
	UCC_GameplayStatics::PrintScreenDebugMessage(FString("Finished Playing timeline"));
}

void ACC_Enemycharacter::SetupRotationTimeline()
{
	if (!IsValid(RotationTimeline)||!IsValid(RotationCurve)) return;
	// RotationTimeline->SetFloatCurve(RotationCurve, "RotationCurve");
	FOnTimelineFloat TimelineProgressDelegate;
	TimelineProgressDelegate.BindUFunction(this, "OnUpdateTimeline");
	FOnTimelineEvent TimelineFinishedDelegate;
	TimelineFinishedDelegate.BindUFunction(this, "OnFinishedTimeline");
	RotationTimeline->AddInterpFloat(RotationCurve, TimelineProgressDelegate);
	RotationTimeline->SetTimelineFinishedFunc(TimelineFinishedDelegate);
}
void ACC_Enemycharacter::RotateTowardsTarget()
{
	if (IsValid(RotationTimeline))
	{
		RotationTimeline->PlayFromStart();
	}else
	{
		UCC_GameplayStatics::PrintScreenDebugMessage(FString("TimelineNOTFOUND"));
	}
}

float ACC_Enemycharacter::GetTimelineLength() const
{
	if (!IsValid(RotationTimeline)) return .4f;
	return RotationTimeline->GetTimelineLength();
}
