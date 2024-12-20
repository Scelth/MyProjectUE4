#pragma once

#define ECC_Bullet ECC_GameTraceChannel1
#define ECC_Climbing ECC_GameTraceChannel2
#define ECC_InteractionVolume ECC_GameTraceChannel3
#define ECC_WallRunnable ECC_GameTraceChannel4

const FName FXParamTraceEnd = FName("TraceEnd");

const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttributes");
const FName DebugCategoryAIAttributes = FName("AIAttributes");
const FName DebugCategoryRangeWeapon = FName("RangeWeapon");

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfileRagdoll = FName("Ragdoll");
const FName CollisionProfileInteractionVolume = FName("PawnInteractionVolume");

const FName PawnHeadBone = FName("head");

const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");
const FName SocketCharacterThrowable = FName("ThrowableSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponForeGrip = FName("ForeGripSocket");

const FName SectionMontageEndReload = FName("EndReload");

const FName BB_CurrentTarget = FName("CurrentTarget");
const FName BB_NextLocation = FName("NextLocation");

const FName ActionInteract = FName("Interact");

UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	None,
	Pistol,
	Rifle,
	SniperRifle,
	Throwable
};

UENUM(BlueprintType)
enum class EAmunitionType : uint8
{
	None,
	Pistol,
	Rifle,
	ShotgunShells,
	FragGrenades,
	RifleGrenades,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlots : uint8
{
	None,
	SideArm,
	PrimaryWeapon,
	SecondaryWeapon,
	PrimaryItemSlot,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EReticleType : uint8
{
	None,
	Default,
	SniperRifle,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETeams : uint8
{
	None,
	Player,
	Neutral,
	Enemy
};