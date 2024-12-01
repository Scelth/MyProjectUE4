#include "BasePlatform.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABasePlatform::ABasePlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = PlatformMesh->GetRelativeLocation();

	if (IsValid(TimelineCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);

		PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);

		if (PlatformBehavior == EPlatformBehavior::Loop)
		{
			PlatformTimeline.Play();
		}
	}
}

void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::PlatformTimelineUpdate(float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);

	if (PlatformBehavior == EPlatformBehavior::Loop && PlatformTimeline.IsPlaying())
	{
		const float PlaybackPosition = PlatformTimeline.GetPlaybackPosition();
		const float TimelineLength = PlatformTimeline.GetTimelineLength();

		if (PlaybackPosition >= TimelineLength && !bIsReversing)
		{
			PlatformTimeline.Stop();
			bIsReversing = true;
			GetWorldTimerManager().SetTimer(LoopDelayTimerHandle, this, &ABasePlatform::HandleLoopDelay, LoopDelay, false);
		}

		else if (PlaybackPosition <= 0.0f && bIsReversing)
		{
			PlatformTimeline.Stop();
			bIsReversing = false;
			GetWorldTimerManager().SetTimer(LoopDelayTimerHandle, this, &ABasePlatform::HandleLoopDelay, LoopDelay, false);
		}
	}
}

void ABasePlatform::HandleLoopDelay()
{
	if (bIsReversing)
	{
		PlatformTimeline.Reverse();
	}

	else
	{
		PlatformTimeline.Play();
	}
}

void ABasePlatform::StartMovement()
{
	if (PlatformBehavior == EPlatformBehavior::OnDemand)
	{
		PlatformTimeline.PlayFromStart();
	}
}