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

private:
	TWeakObjectPtr<AMPAICharacter> CachedAICharacter;
};