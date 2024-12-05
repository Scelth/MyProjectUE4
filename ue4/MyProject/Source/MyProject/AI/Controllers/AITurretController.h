#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AITurretController.generated.h"

class ATurret;

UCLASS()
class MYPROJECT_API AAITurretController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAITurretController();

	virtual void SetPawn(APawn* InPawn) override;
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	//void ReportDamageEventToAI(AActor* DamagedActor, AActor* Instigator, float DamageAmount, FVector EventLocation, FVector HitLocation);

private:

	TWeakObjectPtr<ATurret> CachedTurret;
};
