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
    const FVector LowerPillarPos = LowerPillarMeshComponent->GetRelativeLocation();
    const FVector UpperPillarPos = UpperPillarMeshComponent->GetRelativeLocation();

    FVector CableStart = LowerPillarPos;
    CableStart.Z += PillarHeight * 0.4f;

    FVector CableEnd = UpperPillarPos;
    CableEnd.Z += PillarHeight * 0.4f;

    const FVector CableLocation = (CableStart + CableEnd) * 0.5f;
    const FVector CableDirection = (CableEnd - CableStart).GetSafeNormal();
    const FQuat CableRotation = FQuat::FindBetweenVectors(FVector::ForwardVector, CableDirection);

    UStaticMesh* CableMesh = CableMeshComponent->GetStaticMesh();
    if (IsValid(CableMesh))
    {
        const float MeshWidth = CableMesh->GetBoundingBox().GetSize().X;

        if (!FMath::IsNearlyZero(MeshWidth))
        {
            const float Length = FVector::Dist(CableStart, CableEnd);
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
    const FVector LowerPillarPos = LowerPillarMeshComponent->GetRelativeLocation();
    const FVector UpperPillarPos = UpperPillarMeshComponent->GetRelativeLocation();

    FVector CapsuleStart = LowerPillarPos;
    CapsuleStart.Z += PillarHeight * 0.4f;

    FVector CapsuleEnd = UpperPillarPos;
    CapsuleEnd.Z += PillarHeight * 0.4f;

    const FVector CapsuleLocation = (CapsuleStart + CapsuleEnd) * 0.5f;
    const FVector CapsuleDirection = (CapsuleEnd - CapsuleStart).GetSafeNormal();
    const FQuat CapsuleRotation = FQuat::FindBetweenVectors(FVector::UpVector, CapsuleDirection);
    const float CapsuleHalfHeight = FVector::Dist(CapsuleStart, CapsuleEnd) * 0.5f;

    GetPillarInteractionCapsuleVolume()->SetRelativeLocation(CapsuleLocation);
    GetPillarInteractionCapsuleVolume()->SetRelativeRotation(CapsuleRotation);
    GetPillarInteractionCapsuleVolume()->SetCapsuleHalfHeight(CapsuleHalfHeight);
}

UCapsuleComponent* AZipline::GetPillarInteractionCapsuleVolume() const
{
    return StaticCast<UCapsuleComponent*>(InteractionVolume);
}