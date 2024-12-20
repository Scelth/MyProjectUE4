#pragma once

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "AITurretController.generated.h"

class ATurret;

UCLASS()
class MYPROJECT_API AAITurretController : public AMPAIController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override;
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

private:
	TWeakObjectPtr<ATurret> CachedTurret;
};
