#include "LedgeDetectorComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "CollisionQueryParams.h"
#include "MyProject/MPTypes.h"
#include "DrawDebugHelpers.h"
#include "MyProject/Utils/MPTraceUtils.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Subsystems/DebugSubsystem.h"

void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ACharacter>(), TEXT("ULedgeDetectorComponent::BeginPlay() only character can use ULedgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());

	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	BaseCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	BaseCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryLedgeDetection);
#else
	bool bIsDebugEnabled = false;
#endif

	float DrawTime = 2.f;
	float BottomZOffset = 2.f;
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (BaseCapsuleHalfHeight - BottomZOffset) * FVector::UpVector;

	// Forward
	FHitResult ForwardCheckHitResult;

	float ForwardCheckCapsuleRadius = BaseCapsuleRadius;
	float ForwardCheckCapsuleHalfHeight = (MaxLedgeHeight - MinLedgeHeight) * 0.5f;

	FVector ForwardStartLocation = CharacterBottom + (MinLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;

	if (!MPTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}


	// Downward
	FHitResult DownwardCheckHitResult;

	float DownwardCheckSphereRadius = BaseCapsuleRadius;
	float DownwardCheckDepthOffset = 10.f;

	FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardStartLocation.Z = CharacterBottom.Z + MaxLedgeHeight + DownwardCheckSphereRadius;
	FVector DownwardEndLocation(DownwardStartLocation.X, DownwardStartLocation.Y, CharacterBottom.Z);

	if (!MPTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardCheckHitResult, DownwardStartLocation, DownwardEndLocation, DownwardCheckSphereRadius, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}


	// Overlap
	float OverlapCapsuleRadius = BaseCapsuleRadius;
	float OverlapCapsuleHalfHeight = BaseCapsuleHalfHeight;
	float OverlapCapsuleOffset = 2.f;

	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleOffset) * FVector::UpVector;

	if (MPTraceUtils::OverlapCapusleBlockingByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bIsDebugEnabled, DrawTime))
	{
		return false;
	}

	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.f, -1.f, 0.f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal;


	return true;
}