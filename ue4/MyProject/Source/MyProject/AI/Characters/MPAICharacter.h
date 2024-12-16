#pragma once

#include "CoreMinimal.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MPAICharacter.generated.h"

class UAIPatrolingComponent;
class UBehaviorTree;
class UWidgetComponent;

UCLASS(Blueprintable)
class MYPROJECT_API AMPAICharacter : public AMPBaseCharacter
{
	GENERATED_BODY()
	
public:
	AMPAICharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UAIPatrolingComponent* GetPatrolingComponent() const { return AIPatrolingComponent; }
	UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPatrolingComponent* AIPatrolingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleDefaultsOnly, Category = "Character | Components")
	UWidgetComponent* HealthBarProgressComponent;

	void InitializeHealthProgress();
};