#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MPPlayerController.generated.h"

UCLASS()
class MYPROJECT_API AMPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override;

protected:
	virtual void SetupInputComponent() override;

private:
	TSoftObjectPtr<class AMPBaseCharacter> CachedBaseCharacter;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void ChangeCrouchState();
	void ChangeProneState();
	void Mantle();
	void Jump();
	void StartSprint();
	void StopSprint();
};
