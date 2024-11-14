#include "WeaponBarellComponent.h"
#include "MyProject/Subsystems/DebugSubsystem.h"
#include <MyProject/MPTypes.h>
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, AController* Controller, float SpreadAngle)
{
	for (int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.f, SpreadAngle), ShotDirection.ToOrientationRotator());

		FVector MuzzleLocation = GetComponentLocation();
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubSystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
#else
		bool bIsDebugEnabled = false;
#endif

		FHitResult ShotResult;

		if (GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet))
		{
			ShotEnd = ShotResult.ImpactPoint;

			if (bIsDebugEnabled)
			{
				DrawDebugSphere(GetWorld(), ShotEnd, 10.f, 24, FColor::Red, false, 1.f);
			}

			AActor* HitActor = ShotResult.GetActor();

			if (IsValid(HitActor))
			{
				float Distance = FVector::Distance(ShotStart, ShotEnd);
				float DamageFactor = FalloffDiagram ? FalloffDiagram->GetFloatValue(Distance) : DamageMultiplier;
				float FinalDamage = DamageAmount * DamageFactor;

				FPointDamageEvent DamageEvent;
				DamageEvent.HitInfo = ShotResult;
				DamageEvent.ShotDirection = ShotDirection;
				DamageEvent.DamageTypeClass = DamageTypeClass;

				HitActor->TakeDamage(FinalDamage, DamageEvent, Controller, GetOwner());
			}

			UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, ShotResult.ImpactPoint, ShotResult.ImpactNormal.ToOrientationRotator());

			if (IsValid(DecalComponent))
			{
				DecalComponent->SetFadeScreenSize(DecalFadeOutScreenSize);
				DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
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

FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.f, 2 * PI);
	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;
}
