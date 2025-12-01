// Papontron Inc. All Rigths reserved.


#include "Actors/CC_EnemyProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Characters/CC_PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Tags/CC_GameplayTags.h"


// Sets default values
ACC_EnemyProjectile::ACC_EnemyProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	RootComponent = ProjectileMesh;
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>("ParticleSystemComponent");
	ParticleSystemComponent->SetupAttachment(ProjectileMesh);
	
}

void ACC_EnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(10.f);
	//Spawn Muzzle vfx
	UGameplayStatics::SpawnEmitterAtLocation(this, MuzzleParticle,GetActorLocation());	
}

void ACC_EnemyProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	ACC_PlayerCharacter* HitPlayer = Cast<ACC_PlayerCharacter>(OtherActor);
	if (!IsValid(HitPlayer) || !HitPlayer->IsAlive() || ! HasAuthority()) return;
	FVector HitLocation = HitPlayer->GetActorLocation();
	//Spawn Effects:
	SpawnProjectileEffects(HitLocation);
	UAbilitySystemComponent* ASC = HitPlayer->GetAbilitySystemComponent();
	if (!ASC) return;
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, CCTags::SetByCaller::Projectile,Damage);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	//Apply Gameplay Effect;
	OnImpact(HitPlayer, HitLocation);
	Destroy();
}

void ACC_EnemyProjectile::SpawnProjectileEffects(FVector& Location)
{
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, Location);
	}
	
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
	}
}



