#include "AIPatrolingComponent.h"
#include "MyProject/Actors/Navigation/PatrolingPath.h"

bool UAIPatrolingComponent::CanPatrol() const
{
	return IsValid(PatrolSettings.PatrolingPath) && PatrolSettings.PatrolingPath->GetWayPoints().Num() > 0;
}

FVector UAIPatrolingComponent::SelectClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	FTransform PathTransform = PatrolSettings.PatrolingPath->GetActorTransform();
	const TArray<FVector> WayPoints = PatrolSettings.PatrolingPath->GetWayPoints();

	FVector ClosestWayPoint;
	float MinSquareDistance = FLT_MAX;

	for (int32 i = 0; i < WayPoints.Num(); ++i)
	{
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentSquareDistance = (OwnerLocation - WayPointWorld).SizeSquared();

		if (CurrentSquareDistance < MinSquareDistance)
		{
			ClosestWayPoint = WayPointWorld;
			CurrentWayPointIndex = i;
		}
	}

	return ClosestWayPoint;
}

FVector UAIPatrolingComponent::SelectNextWayPoint()
{
    const TArray<FVector>& WayPoints = PatrolSettings.PatrolingPath->GetWayPoints();
    FTransform PathTransform = PatrolSettings.PatrolingPath->GetActorTransform();

    if (PatrolSettings.PatrolMode == EPatrolMode::Circle)
    {
        CurrentWayPointIndex = (CurrentWayPointIndex + 1) % WayPoints.Num();
    }

    else if (PatrolSettings.PatrolMode == EPatrolMode::PingPong)
    {
        if (CurrentWayPointIndex == 0 && Direction == -1)
        {
            Direction = 1;
        }

        else if (CurrentWayPointIndex == WayPoints.Num() - 1 && Direction == 1)
        {
            Direction = -1;
        }

        CurrentWayPointIndex += Direction;
    }

    return PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
}