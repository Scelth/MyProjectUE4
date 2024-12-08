#include "MPAICharacterController.h"
#include "MyProject/AI/Characters/MPAICharacter.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "MyProject/Components/CharacterComponents/AIPatrolingComponent.h"

void AMPAICharacterController::BeginPlay()
{
	Super::BeginPlay();

	UAIPatrolingComponent* PatrollingComponent = CachedAICharacter->GetPatrolingComponent();

	if (PatrollingComponent->CanPatrol())
	{
		FVector ClosestWayPoint = PatrollingComponent->SelectClosestWayPoint();
		MoveToLocation(ClosestWayPoint);
		bIsPatrolling = true;
	}
}

void AMPAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (IsValid(InPawn))
	{
		CachedAICharacter = StaticCast<AMPAICharacter*>(InPawn);
	}

	else
	{
		CachedAICharacter = nullptr;
	}
}

void AMPAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);

	if (!CachedAICharacter.IsValid())
	{
		return;
	}

	TryMoveToNextTarget();
}

void AMPAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (!Result.IsSuccess())
	{
		return;
	}

	TryMoveToNextTarget();
}

bool AMPAICharacterController::IsTargetReached(FVector TargetLocation) const
{
	return (TargetLocation - CachedAICharacter->GetActorLocation()).SizeSquared() <= FMath::Square(TargetReachRadius);
}

void AMPAICharacterController::TryMoveToNextTarget()
{
	UAIPatrolingComponent* PatrollingComponent = CachedAICharacter->GetPatrolingComponent();

	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	AActor* DamagedActor = GetClosestSensedActor(UAISense_Damage::StaticClass());

	if (IsValid(DamagedActor))
	{
		ClosestActor = DamagedActor;
	}

	if (IsValid(ClosestActor))
	{
		if (!IsTargetReached(ClosestActor->GetActorLocation()))
		{
			MoveToActor(ClosestActor);
		}
		
		bIsPatrolling = false;
	}

	else if(PatrollingComponent->CanPatrol())
	{
		FVector WayPoint = bIsPatrolling ? PatrollingComponent->SelectNextWayPoint() : PatrollingComponent->SelectClosestWayPoint();

		if (!IsTargetReached(WayPoint))
		{
			MoveToLocation(WayPoint);
		}

		bIsPatrolling = true;
	}
}