// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TimelineComponent.h>
#include "MPBasePlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformBehavior : uint8
{
	OnDemand = 0,
	Loop
};

UCLASS()
class MYPROJECT_API AMPBasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	AMPBasePlatform();

protected:
    FTimeline PlatformTimeline;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* PlatformMesh;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
    FVector EndLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
    FVector StartLocation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UCurveFloat* TimelineCurve;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float LoopWaitTime = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
    bool bIsMovingForward = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
    FTimerHandle LoopWaitTimerHandle;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Platform Movement")
    void MoveForward();

    // Function to start moving the platform backward
    UFUNCTION(BlueprintCallable, Category = "Platform Movement")
    void MoveBackward();

private:
    void PlatformTimelineUpdate(float DeltaTime);
    void StartLoopMovement();
    void ReverseLoopMovement();
    void WaitAtEndpoint();
};
