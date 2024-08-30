// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MPBasePawnMovementComponent.generated.h"


UCLASS()
class MYPROJECT_API UMPBasePawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

public:
	void JumpStart();

protected:
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1200.f;

	UPROPERTY(EditAnywhere)
	float InitialJumpVelocity = 400.f;

	UPROPERTY(EditAnywhere)
	bool bEnableGravity;

private:
	FVector VerticalVelocity = FVector::ZeroVector;
	bool bIsFalling = false;
};