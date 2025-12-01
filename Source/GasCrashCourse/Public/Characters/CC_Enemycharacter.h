// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "CC_BaseCharacter.h"
#include "CC_Enemycharacter.generated.h"

class ACC_PlayerCharacter;
class UTimelineComponent;
class UCurveFloat;
UCLASS()
class GASCRASHCOURSE_API ACC_Enemycharacter : public ACC_BaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACC_Enemycharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	
	
	UPROPERTY(VisibleDefaultsOnly)
	UTimelineComponent* RotationTimeline;
	void RotateTowardsTarget();
	float GetTimelineLength()const;
#pragma region CustomMethods
	virtual UAttributeSet* GetAttributeSet() const override;
	float GetRadiusAcceptance() const { return RadiusAcceptance; }
	float GetMinAttackRate() const {return MinAttackRate;}
	float GetMaxAttackRate() const { return MaxAttackRate;}
	float GetSearchRate() const { return SearchRate;}
	float GetSearchRadius() const { return SearchRadius;}
	float GetAttackRange() const{ return AttackRange;}

	void SetTargetActor(AActor* NewTargetActor);
	AActor* GetTargetActor()const;
	
#pragma endregion CustomMethods
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
#pragma region CustomMethods
	virtual void HandleRespawn() override;
	virtual void HandleDeath() override;

	
#pragma endregion CustomMethods
private:

#pragma region Setup
	
	
	UPROPERTY(EditDefaultsOnly, Category="Setup|Times")
	float RespawnTimeSeconds = 5.f;
	UPROPERTY(EditAnywhere, Category="Setup|AI")
	float RadiusAcceptance = 100.f;
	UPROPERTY(EditAnywhere, Category="Setup|AI")
	float MinAttackRate = 0.4f;
	UPROPERTY(EditAnywhere, Category="Setup|AI")
	float MaxAttackRate = 1.f;
	UPROPERTY(EditAnywhere, Category="Setup|AI")
	float SearchRate = 3.f;
	UPROPERTY(EditAnywhere, Category="Setup|AI")
	float SearchRadius = 2000.f;
	UPROPERTY(EditAnywhere, Category="Setup|Combat")
	float AttackRange = 700.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup|Curve")
	TObjectPtr<UCurveFloat> RotationCurve;
#pragma endregion
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	FTimerHandle RespawnTimerHandle;
	void OnUpdateTimeline(float Alpha);
	void OnFinishedTimeline();
	
	void SetupRotationTimeline();

	TWeakObjectPtr<AActor> TargetActor;
};
