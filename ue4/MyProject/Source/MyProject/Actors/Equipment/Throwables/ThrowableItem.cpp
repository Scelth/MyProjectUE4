#include "ThrowableItem.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Actors/Projectiles/MPProjectile.h"

void AThrowableItem::Throw()
{
    if (GetThrowableCount() == 0)
    {
        return;
    }

    checkf(GetOwner()->IsA<AMPBaseCharacter>(), TEXT("AThrowableItem::Throw() only character can be an owner of range throwables"));
    AMPBaseCharacter* CharacterOwner = StaticCast<AMPBaseCharacter*>(GetOwner());

    APlayerController* Controller = CharacterOwner->GetController<APlayerController>();

    if (!IsValid(Controller))
    {
        return;
    }

    FVector PlayerViewPoint;
    FRotator PlayerViewRotation;

    Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
    FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);

    FVector PlayerViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
    FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);
    FVector LaunchDirection = PlayerViewDirection + FMath::Tan(FMath::RadiansToDegrees(ThrowAngle)) * ViewUpVector;
    FVector ThorwableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
    FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThorwableSocketLocation);
    FVector SpawnLocation = PlayerViewPoint + PlayerViewDirection * SocketInViewSpace.X + PlayerViewDirection.RightVector * ThrowOffsetX;

    AMPProjectile* Projectile = GetWorld()->SpawnActor<AMPProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);

    if (IsValid(Projectile))
    {
        Projectile->SetOwner(GetOwner());
        Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
    }
}

void AThrowableItem::SetThrowableCount(int32 NewThrowableCount)
{
    ThrowableCount = NewThrowableCount;

    if (OnThrowableCountChangedEvent.IsBound())
    {
        OnThrowableCountChangedEvent.Broadcast(ThrowableCount);
    }
}