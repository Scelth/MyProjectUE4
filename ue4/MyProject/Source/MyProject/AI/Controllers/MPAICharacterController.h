#pragma once

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "MPAICharacterController.generated.h"

class AMPAICharacter;

UCLASS()
class MYPROJECT_API AMPAICharacterController : public AMPAIController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override;
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float TargetReachRadius = 100.f;

private:
	bool bIsPatrolling = false;

	TWeakObjectPtr<AMPAICharacter> CachedAICharacter;

	bool IsTargetReached(FVector TargetLocation) const;
	void TryMoveToNextTarget();
};