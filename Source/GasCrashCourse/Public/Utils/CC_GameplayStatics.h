// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CC_GameplayStatics.generated.h"

UENUM(BlueprintType)
enum class EHitDirection: uint8
{
	Front UMETA(DisplayName = "Front"),
	Back UMETA(DisplayName = "Back"),
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

USTRUCT(BlueprintType)
struct FFindClosestTargetResult
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> Target = nullptr;
	UPROPERTY(BlueprintReadOnly)
	float Distance = 0;
};
UCLASS()
class GASCRASHCOURSE_API UCC_GameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "CC Utils")
	static EHitDirection GetHitDirectionFromInstigator(const AActor* ActorHit, const AActor* Instigator, bool bDebug = false);
	UFUNCTION(BlueprintCallable, Category = "CC Utils")
	static FName GetHitReactMontageSectionName(EHitDirection HitDirection);
	static void PrintScreenDebugMessage(const FString& Message, float Duration = 10.f, FColor Color = FColor::Emerald );

	static void DrawDebugSphere(const UObject* UObject, const FVector& Center, float Radius,float Duration=5.f, FColor Color = FColor::Blue);

	static void DrawDebugArrow(const UObject* UObject, const FVector& Origin, const FVector& End, float Duration=5.f, FColor Color = FColor::Red);

	static int GetRandomArrayIndex(const int ArraySize);

	static FFindClosestTargetResult FindClosesTargetWithTag(const UObject* WorldContextObject,const FVector& Origin, FName TargetTag);

	
};
