#include "AITurretController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "MyProject/AI/Characters/Turret.h"

AAITurretController::AAITurretController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("TurretPerception"));
}

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
	if (!CachedTurret.IsValid())
	{
		return;
	}

	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), SeenActors);
	AActor* ClosestActor = nullptr;
	FVector TurretLocation = CachedTurret->GetActorLocation();
	float MinSquaredDistance = FLT_MAX;

	for (AActor* SeenActor : SeenActors)
	{
		float CurrentSquareDistance = FVector::DistSquared(TurretLocation, SeenActor->GetActorLocation());

		if (CurrentSquareDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquareDistance;
			ClosestActor = SeenActor;
		}
	}

	CachedTurret->SetCurrentTarget(ClosestActor);
}

//void AAITurretController::ReportDamageEventToAI(AActor* DamagedActor, AActor* Instigator, float DamageAmount, FVector EventLocation, FVector HitLocation)
//{
//	UAISense_Damage::ReportDamageEvent(GetWorld(), DamagedActor, Instigator, DamageAmount, EventLocation, HitLocation);
//}