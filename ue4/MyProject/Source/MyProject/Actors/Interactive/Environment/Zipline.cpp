#include "Zipline.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "MyProject/MPTypes.h"

AZipline::AZipline()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PillarRoot"));

    LowerPillarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftPillar"));
    LowerPillarMeshComponent->SetupAttachment(RootComponent);

    UpperPillarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightPillar"));
    UpperPillarMeshComponent->SetupAttachment(RootComponent);

    CableMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cable"));
    CableMeshComponent->SetupAttachment(RootComponent);

    InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
    InteractionVolume->SetupAttachment(RootComponent);
    InteractionVolume->SetCollisionProfileName(CollisionProfileInteractionVolume);
    InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::OnConstruction(const FTransform& Transform)
{
    const float AngleRadians = FMath::DegreesToRadians(UpperPillarAngleDegrees);
    const float UpperPillarY = CableLength * 0.5f * FMath::Cos(AngleRadians);
    const float UpperPillarZ = CableLength * 0.5f * FMath::Sin(AngleRadians) + PillarHeight * 0.5f;

    UpperPillarMeshComponent->SetRelativeLocation(FVector(0.f, UpperPillarY, UpperPillarZ));
    LowerPillarMeshComponent->SetRelativeLocation(FVector(0.f, -CableLength * 0.5f, PillarHeight * 0.5f));

    ScalePillar(LowerPillarMeshComponent);
    ScalePillar(UpperPillarMeshComponent);
    ScaleCable();
    ScaleInteractionCapsule();
}

void AZipline::ScalePillar(UStaticMeshComponent* PillarMeshComponent)
{
    UStaticMesh* PillarMesh = PillarMeshComponent->GetStaticMesh();

    if (IsValid(PillarMesh))
    {
        float MeshHeight = PillarMesh->GetBoundingBox().GetSize().Z;

        if (!FMath::IsNearlyZero(MeshHeight))
        {
            PillarMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, PillarHeight / MeshHeight));
        }
    }
}

void AZipline::ScaleCable()
{
    const FVector LeftPillarPos = LowerPillarMeshComponent->GetRelativeLocation();
    const FVector RightPillarPos = UpperPillarMeshComponent->GetRelativeLocation();
    const FVector CableLocation = (LeftPillarPos + RightPillarPos) * 0.5f;
    const FVector CableDirection = (RightPillarPos - LeftPillarPos).GetSafeNormal();
    const FQuat CableRotation = FQuat::FindBetweenVectors(FVector::ForwardVector, CableDirection);

    UStaticMesh* StepsMesh = CableMeshComponent->GetStaticMesh();

    if (IsValid(StepsMesh))
    {
        float MeshWidth = StepsMesh->GetBoundingBox().GetSize().X;

        if (!FMath::IsNearlyZero(MeshWidth))
        {
            float Length = FVector::Dist(LeftPillarPos, RightPillarPos);
            CableMeshComponent->SetRelativeScale3D(FVector(Length / MeshWidth, 1.f, 1.f));
        }
    }

    CableMeshComponent->SetRelativeLocation(CableLocation);
    CableMeshComponent->SetRelativeRotation(CableRotation);
    CableMeshComponent->ClearInstances();
    CableMeshComponent->AddInstance(FTransform(FVector(1.f, 1.f, 1.f)));
}

void AZipline::ScaleInteractionCapsule()
{
    const FVector LeftPillarPos = LowerPillarMeshComponent->GetRelativeLocation();
    const FVector RightPillarPos = UpperPillarMeshComponent->GetRelativeLocation();
    const FVector CapsuleLocation = (LeftPillarPos + RightPillarPos) * 0.5f;
    const FVector CapsuleDirection = (RightPillarPos - LeftPillarPos).GetSafeNormal();
    const FQuat CapsuleRotation = FQuat::FindBetweenVectors(FVector::UpVector, CapsuleDirection);
    const float CapsuleHalfHeight = FVector::Dist(LeftPillarPos, RightPillarPos) * 0.5f;

    GetPillarInteractionCapsuleVolume()->SetRelativeLocation(CapsuleLocation);
    GetPillarInteractionCapsuleVolume()->SetRelativeRotation(CapsuleRotation);
    GetPillarInteractionCapsuleVolume()->SetCapsuleHalfHeight(CapsuleHalfHeight);
}

UCapsuleComponent* AZipline::GetPillarInteractionCapsuleVolume() const
{
    return StaticCast<UCapsuleComponent*>(InteractionVolume);
}