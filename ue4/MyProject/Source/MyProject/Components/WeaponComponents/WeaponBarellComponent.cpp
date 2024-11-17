#include "WeaponBarellComponent.h"
#include "MyProject/Subsystems/DebugSubsystem.h"
#include "MyProject/MPTypes.h"
#include "MyProject/Actors/Projectiles/MPProjectile.h"
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

	for (int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.f, SpreadAngle), ShotDirection.ToOrientationRotator());

		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubSystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
#else
		bool bIsDebugEnabled = false;
#endif

		switch (HitRegistrationType)
		{
			case EHitRegistrationType::HitScan: 
			{
				bool bHasHit = HitScan(ShotStart, ShotEnd, ShotDirection);

				if (bIsDebugEnabled && bHasHit)
				{
					DrawDebugSphere(GetWorld(), ShotEnd, 10.f, 24, FColor::Red, false, 1.f);
				}
			
				break;
			}

			case EHitRegistrationType::Projectile:
			{
				LaunchProjectile(ShotStart, ShotDirection);

				break;
			}
		}


		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);

		if (bIsDebugEnabled)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.f, 0, 3.f);
		}
	}
}

bool UWeaponBarellComponent::HitScan(const FVector& ShotStart, OUT FVector& ShotEnd, const FVector& ShotDirection)
{
	FHitResult ShotResult;

	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);

	if (bHasHit)
	{
		ShotEnd = ShotResult.ImpactPoint;

		ProcessHit(ShotResult, ShotDirection/*, ShotStart, ShotEnd*/);
	}

	return bHasHit;
}

void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction/*, const FVector& ShotStart, const FVector& ShotEnd*/)
{
	AActor* HitActor = HitResult.GetActor();

	if (IsValid(HitActor))
	{
		//float Distance = FVector::Distance(ShotStart, ShotEnd);
		//float DamageFactor = FalloffDiagram ? FalloffDiagram->GetFloatValue(Distance) : DamageMultiplier;
		float FinalDamage = DamageAmount /** DamageFactor*/;

		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = DamageTypeClass;

		HitActor->TakeDamage(FinalDamage, DamageEvent, GetController(), GetOwner());
	}

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, HitResult.ImpactPoint, HitResult.ImpactNormal.ToOrientationRotator());

	if (IsValid(DecalComponent))
	{
		DecalComponent->SetFadeScreenSize(DecalFadeOutScreenSize);
		DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
	}
}

void UWeaponBarellComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection)
{
	AMPProjectile* Projectile = GetWorld()->SpawnActor<AMPProjectile>(ProjectileClass, LaunchStart, LaunchDirection.ToOrientationRotator());

	if (IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwningPawn());
		Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessHit);
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}

FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.f, 2 * PI);
	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;
}

APawn* UWeaponBarellComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());

	if (!IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}

	return PawnOwner;
}

AController* UWeaponBarellComponent::GetController() const
{
	APawn* PawnOwner = GetOwningPawn();

	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}
