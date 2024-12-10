#include "BTService_Fire.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "MyProject/Actors/Equipment/Weapons/RangeWeaponItem.h"

UBTService_Fire::UBTService_Fire()
{
	NodeName = "Fire";
}

void UBTService_Fire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(Blackboard))
	{
		return;
	}

	AMPBaseCharacter* Character = Cast<AMPBaseCharacter>(AIController->GetPawn());

	if (!IsValid(Character))
	{
		return;
	}

	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();
	ARangeWeaponItem* RangeWeapon = EquipmentComponent->GetCurrentRangeWeapon();

	if (!IsValid(RangeWeapon))
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!IsValid(CurrentTarget))
	{
		Character->StopFire();
		return;
	}

	float DistanceSquared = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());

	if (DistanceSquared > FMath::Square(MaxFireDistance))
	{
		Character->StopFire();
		return;
	}

	if (!(RangeWeapon->IsRealoading() || RangeWeapon->IsFiring()))
	{
		Character->StartFire();
	}
}