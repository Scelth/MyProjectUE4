#include "AITurretController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "MyProject/AI/Characters/Turret.h"

void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (IsValid(InPawn))
	{
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}

	else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);

    if (!CachedTurret.IsValid())
    {
        return;
    }

    AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
    AActor* DamagedActor = GetClosestSensedActor(UAISense_Damage::StaticClass());

	if (IsValid(DamagedActor))
	{
		ClosestActor = DamagedActor;
	}

    CachedTurret->SetCurrentTarget(ClosestActor);
}