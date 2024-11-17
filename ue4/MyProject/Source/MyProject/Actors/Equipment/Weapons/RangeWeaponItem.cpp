#include "RangeWeaponItem.h"
#include "MyProject/Components/WeaponComponents/WeaponBarellComponent.h"
#include <MyProject/MPTypes.h>
#include <MyProject/Characters/MPBaseCharacter.h>

ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

	EquippedSocketName = SocketCharacterWeapon;

	ReticleType = EReticleType::Default;
}

void ARangeWeaponItem::StartFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}

	bIsFiring = true;

	MakeShot();
}

void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
}

void ARangeWeaponItem::StartAim()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAim()
{
	bIsAiming = false;
}

void ARangeWeaponItem::StartReload()
{
	checkf(GetOwner()->IsA<AMPBaseCharacter>(), TEXT("ARangeWeaponItem::StartReload() only character can be an owner of range weapon"));
	AMPBaseCharacter* CharacterOwner = StaticCast<AMPBaseCharacter*>(GetOwner());

	bIsReloading = true;

	if (IsValid(CharacterReloadMontage))
	{
		float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);

		PlayAnimMontage(WeaponReloadMontage);

		if (ReloadType == EReloadType::FullClip)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]() { EndReload(true); }, MontageDuration, false);
		}
	}

	else
	{
		EndReload(true);
	}
}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	if (!bIsReloading)
	{
		return;
	}

	if (!bIsSuccess)
	{
		checkf(GetOwner()->IsA<AMPBaseCharacter>(), TEXT("ARangeWeaponItem::StartReload() only character can be an owner of range weapon"));
		AMPBaseCharacter* CharacterOwner = StaticCast<AMPBaseCharacter*>(GetOwner());

		CharacterOwner->StopAnimMontage(CharacterReloadMontage);

		StopAnimMontage(WeaponReloadMontage, false);
	}

	if (ReloadType == EReloadType::ByBullet)
	{
		AMPBaseCharacter* CharacterOwner = StaticCast<AMPBaseCharacter*>(GetOwner());
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

		if (IsValid(CharacterAnimInstance))
		{
			CharacterAnimInstance->Montage_JumpToSection(SectionMontageEndReload, CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();

		if (IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageEndReload, WeaponReloadMontage);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);

	bIsReloading = false;

	if (bIsSuccess && OnReloadCompleteEvent.IsBound())
	{
		OnReloadCompleteEvent.Broadcast();
	}
}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
	Ammo = NewAmmo;

	if (OnAmmoChangedEvent.IsBound())
	{
		OnAmmoChangedEvent.Broadcast(Ammo);
	}
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
	return bIsAiming ? AimReticleType : ReticleType;
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();

	SetAmmo(MaxAmmo);
}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float AngleInDegrees = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegrees);
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();

	float Result = 0.f;

	if (IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(AnimMontage);
	}

	return Result;
}

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();

	if (IsValid(WeaponAnimInstance))
	{
		WeaponAnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
	}
}

void ARangeWeaponItem::MakeShot()
{
	checkf(GetOwner()->IsA<AMPBaseCharacter>(), TEXT("ARangeWeaponItem::Attack() only character can be an owner of range weapon"));
	AMPBaseCharacter* CharacterOwner = StaticCast<AMPBaseCharacter*>(GetOwner());

	if (!CanShoot())
	{
		StopFire();

		if (Ammo == 0 && bIsAutoReload)
		{
			CharacterOwner->ReloadCurrentRangeWeapon();
		}

		return;
	}

	EndReload(false);

	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);

	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();

	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;

	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);

	FVector PlayerViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);

	SetAmmo(Ammo - 1);

	WeaponBarell->Shot(PlayerViewPoint, PlayerViewDirection, GetCurrentBulletSpreadAngle());

	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
}

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if (!bIsFiring)
	{
		return;
	}

	switch (WeaponFireMode)
	{
		case EWeaponFireMode::Single:
		{
			StopFire();
			break;
		}

		case EWeaponFireMode::FullAuto:
		{
			MakeShot();
			break;
		}
	}
}