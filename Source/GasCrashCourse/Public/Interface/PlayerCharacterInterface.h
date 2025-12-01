// Papontron Inc. All Rigths reserved.

#pragma once
#include "GameplayTags.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerCharacterInterface.generated.h"

// This class does not need to be modified.

UINTERFACE()
class UPlayerCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASCRASHCOURSE_API IPlayerCharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void SendHitEventToSelf(const FGameplayTag EventTag, AActor* HitInstigator, FHitResult& HitResult);
	
	
};
