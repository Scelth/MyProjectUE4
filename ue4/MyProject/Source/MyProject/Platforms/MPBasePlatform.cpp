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
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &AMPBasePlatform::PlatformTimelineUpdate);
		PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);
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
}