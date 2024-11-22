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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

private:
	TSoftObjectPtr<class AMPBaseCharacter> CachedBaseCharacter;
	UPlayerHUDWidget* PlayerHUDWidget = nullptr;

	void CreateAndInitializeWidgets();

	// Axis
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);
	void ClimbLadderUp(float Value);

	// Action
	void ChangeCrouchState();
	void ChangeProneState();
	void Mantle();
	void Jump();
	void StartSprint();
	void StopSprint();
	void PlayerStartAttack();
	void PlayerStopAttack();
	void StartAiming();
	void StopAiming();
	void NextItem();
	void PreviousItem();
	void EquipPrimaryItem();
	void Reload();
	void FiringMode();
	void InteractWithLadder();
};