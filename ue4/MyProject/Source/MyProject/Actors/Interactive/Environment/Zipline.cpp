#include "Zipline.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "MyProject/MPTypes.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PillarRoot"));

	LeftPillarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftPillar"));
	LeftPillarMeshComponent->SetupAttachment(RootComponent);

	RightPillarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightPillar"));
	RightPillarMeshComponent->SetupAttachment(RootComponent);

	CableMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cable"));
	CableMeshComponent->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfileInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::BeginPlay()
{
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	RightPillarMeshComponent->SetRelativeLocation(RightPillarLocation);
	LeftPillarMeshComponent->SetRelativeLocation(LeftPillarLocation);

	UStaticMesh* LeftPillarMesh = LeftPillarMeshComponent->GetStaticMesh();

	if (IsValid(LeftPillarMesh))
	{
		float MeshHeight = LeftPillarMesh->GetBoundingBox().GetSize().Z;

		if (!FMath::IsNearlyZero(MeshHeight))
		{
			LeftPillarMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, PillarHeight / MeshHeight));
		}
	}

	UStaticMesh* RightPillarMesh = RightPillarMeshComponent->GetStaticMesh();

	if (IsValid(RightPillarMesh))
	{
		float MeshHeight = RightPillarMesh->GetBoundingBox().GetSize().Z;

		if (!FMath::IsNearlyZero(MeshHeight))
		{
			RightPillarMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, PillarHeight / MeshHeight));
		}
	}

	UStaticMesh* CableMesh = CableMeshComponent->GetStaticMesh();

	if (IsValid(CableMesh))
	{
		float MeshWidth = CableMesh->GetBoundingBox().GetSize().X;

		if (!FMath::IsNearlyZero(MeshWidth))
		{
			CableMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 25.f));
		}
	}

	FVector Center = (RightPillarLocation + LeftPillarLocation) / 2.0f;
	FVector Extent;
	Extent.X = FVector::Dist(RightPillarLocation, LeftPillarLocation) * 0.5f;
	Extent.Y = PillarWidth * 0.5f;
	Extent.Z = PillarHeight * 0.5f;

	FVector BoxDepthExtent = GetPillarInteractionBoxVolume()->GetUnscaledBoxExtent();
	GetPillarInteractionBoxVolume()->SetBoxExtent(Extent);
	GetPillarInteractionBoxVolume()->SetRelativeLocation(Center);

	//FVector TopBoxExtent = TopInteractionVolume->GetUnscaledBoxExtent();
	//TopInteractionVolume->SetBoxExtent(FVector(TopBoxExtent.X, PillarWidth * 0.5f, TopBoxExtent.Z));
	//TopInteractionVolume->SetRelativeLocation(FVector(-TopBoxExtent.X, 0.f, PillarHeight + TopBoxExtent.Z));
}

void AZipline::ScalePillar(UStaticMeshComponent* PillarMeshComponent, float Height, float Width)
{
	if (IsValid(PillarMeshComponent))
	{
		UStaticMesh* PillarMesh = PillarMeshComponent->GetStaticMesh();

		if (IsValid(PillarMesh))
		{
			FVector MeshSize = PillarMesh->GetBoundingBox().GetSize();
			float MeshHeight = MeshSize.Z;
			float MeshWidth = MeshSize.X;

			if (!FMath::IsNearlyZero(MeshHeight) && !FMath::IsNearlyZero(MeshWidth))
			{
				float HeightScale = Height / MeshHeight;
				float WidthScale = Width / MeshWidth;

				PillarMeshComponent->SetRelativeScale3D(FVector(WidthScale, WidthScale, HeightScale));
			}
		}
	}
}

UBoxComponent* AZipline::GetPillarInteractionBoxVolume() const
{
	return StaticCast<UBoxComponent*>(InteractionVolume);
}
