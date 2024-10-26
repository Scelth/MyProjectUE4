#include "MPCharacterAttributesComponent.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Subsystems/DebugSubsystem.h"
#include <MyProject/MPTypes.h>
#include <Kismet/GameplayStatics.h>
#include <DrawDebugHelpers.h>
#include "Components/CapsuleComponent.h"

UMPCharacterAttributesComponent::UMPCharacterAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UMPCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedBaseCharacterOwner = Cast<AMPBaseCharacter>(GetOwner());
	
	Health = MaxHealth;

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

	FVector TextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.f"), Health), nullptr, FColor::Red, 0.f, true);
}
#endif

void UMPCharacterAttributesComponent::OnTakeAnyDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	//UE_LOG(LogDamage, Warning, TEXT("UMPCharacterAttributesComponent::OnTakeAnyDamage %s received %.2f amount of damage from %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	if (Health <= 0.f)
	{
		//UE_LOG(LogDamage, Warning, TEXT("%s is killed by an actir by %s"), *CachedBaseCharacterOwner->GetName(), *DamageCauser->GetName());
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

void UMPCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}

