// Papontron Inc. All Rigths reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CC_TraceNotify.generated.h"

/**
 * 
 */
UCLASS()
class GASCRASHCOURSE_API UCC_TraceNotify : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	
	//virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	//virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
private:
#pragma region Setup
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	FName SocketName;
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TEnumAsByte<ECollisionChannel> HitTraceChannel = ECC_Pawn;
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	bool bDebug = false;
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float TraceRadius = 120.f;
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	FGameplayTag EventTag;
#pragma endregion
	void TraceSphereMultiChannel(const USkeletalMeshComponent* MeshComp, TArray<FHitResult>& HitResults);
	void SendEventToActors(TArray<FHitResult>& HitResults, AActor* Owner);
};
