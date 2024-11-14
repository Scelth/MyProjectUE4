#include "MPProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMPProjectile::AMPProjectile()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	SetRootComponent(CollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 2000.f;
}

void AMPProjectile::LaunchProjectile(FVector Direction)
{
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;

	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);

	OnProjectileLaunched();
}

void AMPProjectile::OnProjectileLaunched() { }