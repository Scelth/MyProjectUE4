#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Fire.generated.h"

UCLASS()
class MYPROJECT_API UBTService_Fire : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Fire();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MaxFireDistance = 800.f;
};
