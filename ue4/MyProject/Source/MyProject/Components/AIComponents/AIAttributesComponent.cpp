#include "AIAttributesComponent.h"
#include "MyProject/AI/Characters/Turret.h"
#include <Kismet/GameplayStatics.h>
#include <DrawDebugHelpers.h>
#include "MyProject/Subsystems/DebugSubsystem.h"
#include <Perception/AISense_Damage.h>

UAIAttributesComponent::UAIAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateHealthValue();

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    DebugDrawAttributes();
#endif
}

void UAIAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedBaseTurretOwner = Cast<ATurret>(GetOwner());

	SetHealth(MaxHealth);

	CachedBaseTurretOwner->OnTakeAnyDamage.AddDynamic(this, &UAIAttributesComponent::OnTakeAnyDamage);
}

void UAIAttributesComponent::OnTakeAnyDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	UAISense_Damage::ReportDamageEvent(
		GetWorld(),
		GetOwner(),
		DamageCauser,
		Damage,
		GetOwner()->GetActorLocation(),
		DamageCauser ? DamageCauser->GetActorLocation() : FVector::ZeroVector);

	UpdateHealthValue();

	if (Health <= 0.f && OnHealthChangedEvent.IsBound())
	{
		OnHealthChangedEvent.Broadcast();
	}
}

void UAIAttributesComponent::UpdateHealthValue()
{
	SetHealth(Health);
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UAIAttributesComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();

	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryAIAttributes))
	{
		return;
	}

	FVector HealthTextLocation = CachedBaseTurretOwner->GetActorLocation();
	DrawDebugString(GetWorld(), HealthTextLocation, FString::Printf(TEXT("Health: %.f"), Health), nullptr, FColor::Red, 0.f, true);
}
#endif