// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "CC_BaseCharacter.h"
#include "Interface/PlayerCharacterInterface.h"
#include "CC_PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
namespace PlayerTag
{
	extern GASCRASHCOURSE_API FName PlayerTag; 
}
UCLASS()
class GASCRASHCOURSE_API ACC_PlayerCharacter : public ACC_BaseCharacter, public IPlayerCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACC_PlayerCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;
	virtual void SendHitEventToSelf_Implementation(const FGameplayTag EventTag, AActor* HitInstigator, FHitResult& HitResult) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
private:
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UCameraComponent> CameraComponent;

	void InitializeAbilitySystemActorInfo();
};
