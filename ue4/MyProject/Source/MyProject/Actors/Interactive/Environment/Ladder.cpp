#include "Ladder.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "MyProject/MPTypes.h"

ALadder::ALadder()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LadderRoot"));

	LeftRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftRail"));
	LeftRailMeshComponent->SetupAttachment(RootComponent);

	RightRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightRail"));
	RightRailMeshComponent->SetupAttachment(RootComponent);

	StepsMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Steps"));
	StepsMeshComponent->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfileInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);

	TopInteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TopInteractionVolume"));
	TopInteractionVolume->SetupAttachment(RootComponent);
	TopInteractionVolume->SetCollisionProfileName(CollisionProfileInteractionVolume);
	TopInteractionVolume->SetGenerateOverlapEvents(true);
}

void ALadder::BeginPlay()
{
	Super::BeginPlay();

	TopInteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractionVolumeOverlapBegin);
	TopInteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractionVolumeOverlapEnd);
}

void ALadder::OnConstruction(const FTransform& Transform)
{
	LeftRailMeshComponent->SetRelativeLocation(FVector(0.f, -LadderWidth * 0.5f, LadderHeight * 0.5f));
	RightRailMeshComponent->SetRelativeLocation(FVector(0.f, LadderWidth * 0.5f, LadderHeight * 0.5f));

	UStaticMesh* LeftRailMesh = LeftRailMeshComponent->GetStaticMesh();
	if (IsValid(LeftRailMesh))
	{
		float MeshHeight = LeftRailMesh->GetBoundingBox().GetSize().Z;

		if (!FMath::IsNearlyZero(MeshHeight))
		{
			LeftRailMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, LadderHeight / MeshHeight));
		}
	}

	UStaticMesh* RighRailMesh = RightRailMeshComponent->GetStaticMesh();
	if (IsValid(RighRailMesh))
	{
		float MeshHeight = RighRailMesh->GetBoundingBox().GetSize().Z;

		if (!FMath::IsNearlyZero(MeshHeight))
		{
			RightRailMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, LadderHeight / MeshHeight));
		}
	}

	UStaticMesh* StepsMesh = StepsMeshComponent->GetStaticMesh();
	if (IsValid(StepsMesh))
	{
		float MeshWidth = StepsMesh->GetBoundingBox().GetSize().X;

		if (!FMath::IsNearlyZero(MeshWidth))
		{
			StepsMeshComponent->SetRelativeScale3D(FVector(LadderWidth / MeshWidth, 1.f, 1.f));
		}
	}

	StepsMeshComponent->ClearInstances();

	uint32 StepsCount = FMath::FloorToInt((LadderHeight - BottomStepOffset) / StepsInterval);

	for (uint32 i = 0; i < StepsCount; ++i)
	{
		FTransform InstanceTransform(FVector(1.f, 1.f, BottomStepOffset + i * StepsInterval));
		StepsMeshComponent->AddInstance(InstanceTransform);
	}

	float BoxDepthExtent = GetLadderInteractionBoxVolume()->GetUnscaledBoxExtent().X;
	GetLadderInteractionBoxVolume()->SetBoxExtent(FVector(BoxDepthExtent, LadderWidth * 0.5f, LadderHeight * 0.5f));
	GetLadderInteractionBoxVolume()->SetRelativeLocation(FVector(BoxDepthExtent, 0.f, LadderHeight * 0.5f));

	FVector TopBoxExtent = TopInteractionVolume->GetUnscaledBoxExtent();
	TopInteractionVolume->SetBoxExtent(FVector(TopBoxExtent.X, LadderWidth * 0.5f, TopBoxExtent.Z));
	TopInteractionVolume->SetRelativeLocation(FVector(-TopBoxExtent.X, 0.f, LadderHeight + TopBoxExtent.Z));
}

FVector ALadder::GetAttachFromTopAnimMontageStartingLocation() const
{
	FRotator OrientationRotation = GetActorForwardVector().ToOrientationRotator();
	FVector Offset = OrientationRotation.RotateVector(AttachFromTopAnimMontageInitialOffset);
	FVector LadderTop = GetActorLocation() + GetActorUpVector() * LadderHeight;
	
	return LadderTop + Offset;
}

void ALadder::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractionVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}

	if (OverlappedComponent == TopInteractionVolume)
	{
		bIsOnTop = true;
	}
}

void ALadder::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}

	if (OverlappedComponent == TopInteractionVolume)
	{
		bIsOnTop = false;
	}
}

UBoxComponent* ALadder::GetLadderInteractionBoxVolume() const
{
	return StaticCast<UBoxComponent*>(InteractionVolume);
}