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

    FVector ThrowLocation;
    FRotator ThrowRotation;

    if (CharacterOwner->IsPlayerControlled())
    {
        APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
        Controller->GetPlayerViewPoint(ThrowLocation, ThrowRotation);
    }

    else
    {
        ThrowLocation = CharacterOwner->GetActorLocation();
        ThrowRotation = CharacterOwner->GetActorRotation();
    }

    FTransform ThrowTransform(ThrowRotation, ThrowLocation);

    FVector ThrowDirection = ThrowRotation.RotateVector(FVector::ForwardVector);
    FVector ViewUpVector = ThrowRotation.RotateVector(FVector::UpVector);
    FVector LaunchDirection = ThrowDirection + FMath::Tan(FMath::RadiansToDegrees(ThrowAngle)) * ViewUpVector;
    FVector ThorwableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
    FVector SocketInViewSpace = ThrowTransform.InverseTransformPosition(ThorwableSocketLocation);
    FVector SpawnLocation = ThrowLocation + ThrowDirection * SocketInViewSpace.X + ThrowDirection.RightVector * ThrowOffsetX;

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