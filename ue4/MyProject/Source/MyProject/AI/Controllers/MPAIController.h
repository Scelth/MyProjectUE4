#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MPAIController.generated.h"

class UAISense;

UCLASS()
class MYPROJECT_API AMPAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMPAIController();

protected:
	AActor* GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const;
};