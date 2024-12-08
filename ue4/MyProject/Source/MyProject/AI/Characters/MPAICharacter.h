#pragma once

#include "CoreMinimal.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MPAICharacter.generated.h"

class UAIPatrolingComponent;

UCLASS(Blueprintable)
class MYPROJECT_API AMPAICharacter : public AMPBaseCharacter
{
	GENERATED_BODY()
	
public:
	AMPAICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrolingComponent* GetPatrolingComponent() const { return AIPatrolingComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPatrolingComponent* AIPatrolingComponent;
};