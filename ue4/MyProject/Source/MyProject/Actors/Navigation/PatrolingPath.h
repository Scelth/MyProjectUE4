#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolingPath.generated.h"

UCLASS()
class MYPROJECT_API APatrolingPath : public AActor
{
	GENERATED_BODY()
	
public:	
	const TArray<FVector>& GetWayPoints() const;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path", meta = (MakeEditWidget))
	TArray<FVector> WayPoints;
};