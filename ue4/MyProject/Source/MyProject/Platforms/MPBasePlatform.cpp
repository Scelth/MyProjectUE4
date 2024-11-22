// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBasePlatform.h"

// Sets default values
AMPBasePlatform::AMPBasePlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

void AMPBasePlatform::BeginPlay()
{
	Super::BeginPlay();
    StartLocation = PlatformMesh->GetRelativeLocation();

    if (IsValid(TimelineCurve))
    {
        FOnTimelineFloat TimelineUpdate;
        TimelineUpdate.BindDynamic(this, &AMPBasePlatform::PlatformTimelineUpdate);
        PlatformTimeline.AddInterpFloat(TimelineCurve, TimelineUpdate);

        if (PlatformBehavior == EPlatformBehavior::Loop)
        {
            StartLoopMovement();
        }
    }
}

void AMPBasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    PlatformTimeline.TickTimeline(DeltaTime);
}

void AMPBasePlatform::PlatformTimelineUpdate(float DeltaTime)
{
    const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, DeltaTime);
    PlatformMesh->SetRelativeLocation(PlatformTargetLocation);

    if (DeltaTime >= 1.0f && bIsMovingForward)
    {
        PlatformTimeline.Stop();
        WaitAtEndpoint();
    }

    else if (DeltaTime <= 0.0f && !bIsMovingForward)
    {
        PlatformTimeline.Stop();
        WaitAtEndpoint();
    }
}

void AMPBasePlatform::MoveForward()
{
    if (PlatformBehavior == EPlatformBehavior::OnDemand)
    {
        bIsMovingForward = true;
        PlatformTimeline.Play();
    }
}

void AMPBasePlatform::MoveBackward()
{
    if (PlatformBehavior == EPlatformBehavior::OnDemand)
    {
        bIsMovingForward = false;
        PlatformTimeline.Reverse();
    }
}

void AMPBasePlatform::StartLoopMovement()
{
    bIsMovingForward = true;
    PlatformTimeline.Play();
}

void AMPBasePlatform::ReverseLoopMovement()
{
    bIsMovingForward = false;
    PlatformTimeline.Reverse();
}

void AMPBasePlatform::WaitAtEndpoint()
{
    GetWorld()->GetTimerManager().SetTimer(
        LoopWaitTimerHandle, [this]()
        {
            if (bIsMovingForward)
            {
                ReverseLoopMovement();
            }

            else
            {
                StartLoopMovement();
            }
        }, LoopWaitTime, false);
}