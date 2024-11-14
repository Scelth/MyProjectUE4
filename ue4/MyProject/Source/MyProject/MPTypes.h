#pragma once

#define ECC_Bullet ECC_GameTraceChannel1
#define ECC_Climbing ECC_GameTraceChannel2

const FName FXParamTraceEnd = FName("TraceEnd");

const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttributes");
const FName DebugCategoryRangeWeapon = FName("RangeWeapon");

const FName PawnCollisionProfile = FName("Pawn");

const FName PawnHeadBone = FName("head");

const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponForeGrip = FName("ForeGripSocket");

const FName CollisionProfileRagdoll = FName("Ragdoll");

const FName SectionMontageEndReload = FName("EndReload");

UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	None,
	Pistol,
	Rifle
};

UENUM(BlueprintType)
enum class EAmunitionType : uint8
{
	none,
	Pistol,
	Rifle,
	ShotgunShells,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlots : uint8
{
	None,
	SideArm,
	PrimaryWeapon,
	SecondaryWeapon,
	MAX UMETA(Hidden)
};