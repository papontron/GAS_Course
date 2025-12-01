// Papontron Inc. All Rigths reserved.


#include "Utils/CC_GameplayStatics.h"

#include "Kismet/GameplayStatics.h"

EHitDirection UCC_GameplayStatics::GetHitDirectionFromInstigator(const AActor* ActorHit, const AActor* Instigator, bool bDebug)
{
	if (!ActorHit || !Instigator)
	{
		if (bDebug) PrintScreenDebugMessage(FString("Invalid actors, unable to calculate hit direction"));
		return EHitDirection::Front;
	}
	FVector ToInstigator = (Instigator->GetActorLocation() - ActorHit->GetActorLocation()).GetSafeNormal();
	const FVector ActorHitForwardVector = ActorHit->GetActorForwardVector();
	const FVector ActorHitRightVector = ActorHit->GetActorRightVector();
	const float ForwardProduct = FVector::DotProduct(ActorHitForwardVector, ToInstigator);
	const float RightProduct = FVector::DotProduct(ActorHitRightVector, ToInstigator);
	const float Tolerance = FMath::Cos(FMath::DegreesToRadians(45.f));
	if (bDebug)
	{
		DrawDebugArrow(ActorHit, ActorHit->GetActorLocation(), Instigator->GetActorLocation());
	}
	if (ForwardProduct > Tolerance)
	{
		return EHitDirection::Front;
	}
	if (ForwardProduct < -Tolerance)
	{
		return EHitDirection::Back;
	}
	if (RightProduct < -Tolerance)
	{
		return EHitDirection::Left;
	}
	if (RightProduct > Tolerance) return EHitDirection::Right;
	return EHitDirection::Front;
}

FName UCC_GameplayStatics::GetHitReactMontageSectionName(EHitDirection HitDirection)
{
	switch (HitDirection)
	{
		case EHitDirection::Front:
		return FName("front");
		case EHitDirection::Back:
		return FName("back");
		case EHitDirection::Left:
		return FName("left");
		case EHitDirection::Right:
		default:
		return FName("right");
		
	}
}

void UCC_GameplayStatics::PrintScreenDebugMessage(const FString& Message, float Duration, FColor Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, *Message);
	}
}

void UCC_GameplayStatics::DrawDebugSphere(const UObject* UObject, const FVector& Center, float Radius,float Duration, FColor Color)
{
	UKismetSystemLibrary::DrawDebugSphere( UObject,Center, Radius, 12, Color, Duration);
}

void UCC_GameplayStatics::DrawDebugArrow(const UObject* UObject, const FVector& Origin, const FVector& End,
	float Duration, FColor Color)
{
	
	UKismetSystemLibrary::DrawDebugArrow(UObject,Origin+FVector(0,0,100.f), End+FVector(0,0,100.f),800.f, Color, Duration);
}

int UCC_GameplayStatics::GetRandomArrayIndex(const int ArraySize)
{
	if (ArraySize == 1) return 0;
	return FMath::RandRange(0,ArraySize-1);
}

FFindClosestTargetResult UCC_GameplayStatics::FindClosesTargetWithTag(const UObject* WorldContextObject,const FVector& Origin, FName TargetTag)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(WorldContextObject, TargetTag, FoundActors);
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (AActor* FoundActor : FoundActors)
	{
		if (!IsValid(FoundActor)) continue;
		if (FVector::Dist(Origin, FoundActor->GetActorLocation()) < ClosestDistance)
		{
			ClosestDistance = FVector::Dist(Origin, FoundActor->GetActorLocation());
			ClosestActor = FoundActor;
		}
	}
	FFindClosestTargetResult Result;
	Result.Distance = ClosestDistance;
	Result.Target = ClosestActor;
	return Result;
}


