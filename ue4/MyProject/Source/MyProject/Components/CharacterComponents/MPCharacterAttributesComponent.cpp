#include "MPCharacterAttributesComponent.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Subsystems/DebugSubsystem.h"
#include <MyProject/MPTypes.h>
#include <Kismet/GameplayStatics.h>
#include <DrawDebugHelpers.h>
#include "Components/CapsuleComponent.h"
#include "MyProject/Components/MovementComponents/MPBaseCharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"

UMPCharacterAttributesComponent::UMPCharacterAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMPCharacterAttributesComponent::SetHealth(float NewHealth)
{
	Health = NewHealth;

	if (OnHealthChangedEvent.IsBound())
	{
		OnHealthChangedEvent.Broadcast(GetHealthPercent());
	}
}

void UMPCharacterAttributesComponent::SetStamina(float NewStamina)
{
	Stamina = NewStamina;

	if (OnStaminaChangedEvent.IsBound())
	{
		OnStaminaChangedEvent.Broadcast(GetStaminaPercent());
	}
}

void UMPCharacterAttributesComponent::SetOxygen(float NewOxygen)
{
	Oxygen = NewOxygen;

	if (OnOxygenChangedEvent.IsBound())
	{
		OnOxygenChangedEvent.Broadcast(GetOxygenPercent());
	}
}

void UMPCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedBaseCharacterOwner = Cast<AMPBaseCharacter>(GetOwner());
	
	SetHealth(MaxHealth);
	SetStamina(MaxStamina);
	SetOxygen(MaxOxygen);

	CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UMPCharacterAttributesComponent::OnTakeAnyDamage);
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UMPCharacterAttributesComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();

	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}

	FVector HealthTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 10.f) * FVector::UpVector;
	DrawDebugString(GetWorld(), HealthTextLocation, FString::Printf(TEXT("Health: %.f"), Health), nullptr, FColor::Red, 0.f, true);
	
	FVector StaminaTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.f) * FVector::UpVector;
	DrawDebugString(GetWorld(), StaminaTextLocation, FString::Printf(TEXT("Stamina: %.f"), Stamina), nullptr, FColor::Green, 0.f, true);

	FVector OxygenTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * FVector::UpVector;
	DrawDebugString(GetWorld(), OxygenTextLocation, FString::Printf(TEXT("Oxygen: %.f"), Oxygen), nullptr, FColor::Blue, 0.f, true);
}
#endif

void UMPCharacterAttributesComponent::OnTakeAnyDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	
	UpdateHealthValue();

	if (Health <= 0.f && OnDeathChangedEvent.IsBound())
	{
		OnDeathChangedEvent.Broadcast();
	}
}

void UMPCharacterAttributesComponent::UpdateHealthValue()
{
	SetHealth(Health);
}

void UMPCharacterAttributesComponent::UpdateStaminaValue(float DeltaTime)
{
	if (!IsAlive())
	{
		return;
	}

	if (Stamina >= MinStaminaToSprint && OnOutOfStaminaChangedEvent.IsBound())
	{
		OnOutOfStaminaChangedEvent.Broadcast(false);
	}

	if (!CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting())
	{
		Stamina = FMath::Clamp(Stamina + StaminaRestoreVelocity * DeltaTime, 0.f, MaxStamina);

		if (Stamina >= MaxStamina && OnOutOfStaminaChangedEvent.IsBound())
		{
			Stamina = MaxStamina;
			OnOutOfStaminaChangedEvent.Broadcast(false);
		}
	}

	else if (CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting() && !CachedBaseCharacterOwner->IsAiming())
	{
		Stamina = FMath::Clamp(Stamina - SprintStaminaConsumptionVelocity * DeltaTime, 0.f, MaxStamina);

		if (Stamina <= 0.f && OnOutOfStaminaChangedEvent.IsBound())
		{
			Stamina = 0.f;
			OnOutOfStaminaChangedEvent.Broadcast(true);
		}
	}

	SetStamina(Stamina);
}

void UMPCharacterAttributesComponent::UpdateOxygenValue(float DeltaTime)
{
	if (!IsAlive())
	{
		return;
	}

	if (CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSwimming())
	{
		if (!IsSwimmingUnderWater())
		{
			Oxygen = FMath::Clamp(Oxygen + SwimOxygenConsumptionVelocity * DeltaTime, 0.f, MaxOxygen);

			if (Oxygen >= MaxOxygen)
			{
				Oxygen = MaxOxygen;
			}
		}

		else if (IsSwimmingUnderWater())
		{
			Oxygen = FMath::Clamp(Oxygen - SwimOxygenConsumptionVelocity * DeltaTime, 0.f, MaxOxygen);

			if (Oxygen <= 0.f)
			{
				Oxygen = 0.f;

				if (GetWorld()->GetTimeSeconds() - LastOxygenDamageTime >= OxygenDamageInterval)
				{
					LastOxygenDamageTime = GetWorld()->GetTimeSeconds();

					OnTakeAnyDamage(CachedBaseCharacterOwner->GetOwner(), OxygenOutDamage, nullptr, nullptr, nullptr);
				}
			}
		}
	}

	SetOxygen(Oxygen);
}

bool UMPCharacterAttributesComponent::IsSwimmingUnderWater() const
{
	FVector HeadPosition = CachedBaseCharacterOwner->GetMesh()->GetSocketLocation(PawnHeadBone);

	APhysicsVolume* Volume = CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->GetPhysicsVolume();
	float VolumeTopPlane = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z;

	if (VolumeTopPlane < HeadPosition.Z)
	{
		return false;
	}

	else 
	{
		return true;
	}
}

void UMPCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHealthValue();
	UpdateStaminaValue(DeltaTime);
	UpdateOxygenValue(DeltaTime);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}