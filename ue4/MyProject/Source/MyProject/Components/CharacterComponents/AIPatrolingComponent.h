#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrolingComponent.generated.h"

class APatrolingPath;

UENUM(BlueprintType)
enum class EPatrolMode : uint8
{
	Circle UMETA(DisplayName = "Circle"),
	PingPong UMETA(DisplayName = "PingPong")
};

USTRUCT(BlueprintType)
struct FPatrolSettings
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	APatrolingPath* PatrolingPath;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	EPatrolMode PatrolMode = EPatrolMode::Circle;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UAIPatrolingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	bool CanPatrol() const;

	FVector SelectClosestWayPoint();
	FVector SelectNextWayPoint();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	FPatrolSettings PatrolSettings;

private:
	int32 CurrentWayPointIndex = -1;
	int32 Direction = 1;
};
