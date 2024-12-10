#include "BTService_Throw.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Actors/Equipment/Throwables/ThrowableItem.h"
#include "MyProject/Components/CharacterComponents/CharacterEquipmentComponent.h"

UBTService_Throw::UBTService_Throw()
{
	NodeName = "Throw";
}

void UBTService_Throw::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!IsValid(CurrentTarget))
	{
		return;
	}

	if (!AIController->LineOfSightTo(CurrentTarget))
	{
		return;
	}

	float DistanceSquared = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());

	if (!(DistanceSquared > FMath::Square(MinThrowDistance) && DistanceSquared < FMath::Square(MaxThrowDistance)))
	{
		return;
	}

	Character->EquipPrimaryItem();
	UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent_Mutable();
	AThrowableItem* Throwable = EquipmentComponent->GetCurrentThrowableItem();

	if (!IsValid(Throwable))
	{
		return;
	}

	if (Throwable->GetThrowableCount() <= 0)
	{
		return;
	}

	EquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
}