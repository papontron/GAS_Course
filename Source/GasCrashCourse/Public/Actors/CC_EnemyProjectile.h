// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CC_EnemyProjectile.generated.h"

class UProjectileMovementComponent;
class UGameplayEffect;
class UArrowComponent;

UCLASS()
class GASCRASHCOURSE_API ACC_EnemyProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACC_EnemyProjectile();

protected:
	virtual void BeginPlay() override;
	// Called when the game starts or when spawned
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	UFUNCTION(BlueprintImplementableEvent, Category = "CC|Projectile")
	void OnImpact(AActor* HitActor, FVector HitLocation);
private:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;
	UPROPERTY(EditDefaultsOnly, Category="Setup|SFX")
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditDefaultsOnly, Category="Setup|GameplayEffect")
	TSubclassOf<UGameplayEffect> GameplayEffect;
	UPROPERTY(EditAnywhere, Category="Setup|Damage", meta=(ClampMax=0))
	float Damage = -20.f;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Particle")
	TObjectPtr<UParticleSystem> ImpactParticle;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Particle")
	TObjectPtr<UParticleSystem> ProjectileTail;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Particle")
	TObjectPtr<UParticleSystem> MuzzleParticle;
	UPROPERTY(EditDefaultsOnly, Category="Setup|Particle", BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UParticleSystem> SecondaryImpactParticle;
	
	void SpawnProjectileEffects(FVector& Location);
};
