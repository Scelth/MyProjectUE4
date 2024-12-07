#include "MPAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"

AMPAIController::AMPAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

AActor* AMPAIController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const
{
    if (!IsValid(GetPawn()))
    {
        return nullptr;
    }

    TArray<AActor*> SensedActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SensedActors);

    AActor* ClosestActor = nullptr;
    FVector PawnLocation = GetPawn()->GetActorLocation();
    float MinSquaredDistance = FLT_MAX;

    for (AActor* Actor : SensedActors)
    {
        float CurrentSquareDistance = FVector::DistSquared(PawnLocation, Actor->GetActorLocation());
        if (CurrentSquareDistance < MinSquaredDistance)
        {
            MinSquaredDistance = CurrentSquareDistance;
            ClosestActor = Actor;
        }
    }

    return ClosestActor;
}
