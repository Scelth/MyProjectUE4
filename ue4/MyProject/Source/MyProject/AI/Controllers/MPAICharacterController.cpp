#include "MPAICharacterController.h"
#include "MyProject/AI/Characters/MPAICharacter.h"
#include "MyProject/Components/CharacterComponents/AIPatrolingComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyProject/MPTypes.h"

void AMPAICharacterController::SetupPatrolling()
{
	UAIPatrolingComponent* PatrollingComponent = CachedAICharacter->GetPatrolingComponent();

	if (PatrollingComponent->CanPatrol())
	{
		FVector ClosestWayPoint = PatrollingComponent->SelectClosestWayPoint();

		if (IsValid(Blackboard))
		{
			Blackboard->SetValueAsVector(BB_NextLocation, ClosestWayPoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		}

		bIsPatrolling = true;
	}
}

void AMPAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (IsValid(InPawn))
	{
		CachedAICharacter = StaticCast<AMPAICharacter*>(InPawn);
		RunBehaviorTree(CachedAICharacter->GetBehaviorTree());
		SetupPatrolling();
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
		if (IsValid(Blackboard))
		{
			Blackboard->SetValueAsObject(BB_CurrentTarget, ClosestActor);
			SetFocus(ClosestActor, EAIFocusPriority::Gameplay);
		}
		
		bIsPatrolling = false;
	}

	else if(PatrollingComponent->CanPatrol())
	{
		FVector WayPoint = bIsPatrolling ? PatrollingComponent->SelectNextWayPoint() : PatrollingComponent->SelectClosestWayPoint();

		if (IsValid(Blackboard))
		{
			ClearFocus(EAIFocusPriority::Gameplay);
			Blackboard->SetValueAsVector(BB_NextLocation, WayPoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		}

		bIsPatrolling = true;
	}
}