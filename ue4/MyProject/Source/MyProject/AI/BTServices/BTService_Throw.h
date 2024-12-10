#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Throw.generated.h"

UCLASS()
class MYPROJECT_API UBTService_Throw : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Throw();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MinThrowDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MaxThrowDistance = 1500.f;
};