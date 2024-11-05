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
}

void ARangeWeaponItem::Fire()
{
	checkf(GetOwner()->IsA<AMPBaseCharacter>(), TEXT("ARangeWeaponItem::Attack() only character can be an owner of range weapon"));
	AMPBaseCharacter* CharacterOwner = StaticCast<AMPBaseCharacter*>(GetOwner());

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

	WeaponBarell->Shot(PlayerViewPoint, PlayerViewDirection, Controller);
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();

	return WeaponAnimInstance->Montage_Play(AnimMontage);
}
