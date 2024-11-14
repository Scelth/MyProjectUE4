#include "ExplosiveProjectile.h"
#include "MyProject/Components/WeaponComponents/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(GetRootComponent());
}

void AExplosiveProjectile::OnProjectileLaunched()
{
	Super::OnProjectileLaunched();

	GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::OnDetonationTimerElapsed, DetonationTime, false);
}

AController* AExplosiveProjectile::GetController()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());

	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}

void AExplosiveProjectile::OnDetonationTimerElapsed()
{
	ExplosionComponent->Explode(GetController());
}