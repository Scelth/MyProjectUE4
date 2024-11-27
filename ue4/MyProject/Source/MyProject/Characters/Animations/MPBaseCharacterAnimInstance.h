#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyProject/MPTypes.h"
#include "MyProject/Components/MovementComponents/MPBaseCharacterMovementComponent.h"
#include "MPBaseCharacterAnimInstance.generated.h"

UCLASS()
class MYPROJECT_API UMPBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
#pragma region Base
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation", meta = (UIMin = 0.f, UIMax = 500.f))
	float Speed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation", meta = (UIMin = -180.f, UIMax = 180.f))
	float Direction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsStrafing = false;
#pragma endregion

#pragma region Jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsFalling = false;
#pragma endregion

#pragma region Sprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOutOfStamina = false;
#pragma endregion

#pragma region Crouch and Prone
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsProning = false;
#pragma endregion

#pragma region Swim
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsSwimming = false;
#pragma endregion
	
#pragma region Ladder
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float LadderSpeedRatio = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOnLadder = false;	
#pragma endregion

#pragma region WallRun
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	EWallRunSide WallRunSide;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOnWall = false;
#pragma endregion

#pragma region Inverse Kinematic
	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
	FVector IKRightFootOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
	FVector IKLeftFootOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
	FVector IKPelvisBoneOffset = FVector::ZeroVector;
#pragma endregion

#pragma region Weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsAiming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	FRotator AimRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation | Weapon")
	EEquipableItemType CurrentEquippedItem = EEquipableItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation | Weapon")
	FTransform ForeGripSocketTransforn;
#pragma endregion

private:
	TWeakObjectPtr<class AMPBaseCharacter> CachedBaseCharacter;
};
