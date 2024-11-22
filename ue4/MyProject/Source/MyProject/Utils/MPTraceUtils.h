#pragma once

namespace MPTraceUtils
{
	bool SweepCapsuleSingleByChannel(const UWorld* World,
		struct FHitResult& OutHit,
		const FVector& Start,
		const FVector& End,
		float CapsuleRadius,
		float CapsuleHalfHeight,
		const FQuat& Rotation,
		ECollisionChannel TraceChannel,
		const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
		const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam,
		bool bDrawDebug = false,
		float DrawTime = -1.f,
		FColor TraceColor = FColor::Black,
		FColor HitColor = FColor::Red);

	bool SweepSphereSingleByChannel(const UWorld* World,
		struct FHitResult& OutHit,
		const FVector& Start,
		const FVector& End,
		float Radius,
		ECollisionChannel TraceChannel,
		const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
		const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam,
		bool bDrawDebug = false,
		float DrawTime = -1.f,
		FColor TraceColor = FColor::Black,
		FColor HitColor = FColor::Red);

	bool OverlapCapsuleAnyByProfile(const UWorld* World,
		const FVector& Position,
		float CapsuleRadius,
		float CapsuleHalfHeight,
		FQuat Rotation,
		FName ProfileName,
		const FCollisionQueryParams& QueryParams,
		bool bDrawDebug = false,
		float DrawTime = -1.f,
		FColor HitColor = FColor::Red);

	bool OverlapCapsuleBlockingByProfile(const UWorld* World,
		const FVector& Position,
		float CapsuleRadius,
		float CapsuleHalfHeight,
		FQuat Rotation,
		FName ProfileName,
		const FCollisionQueryParams& QueryParams,
		bool bDrawDebug = false,
		float DrawTime = -1.f,
		FColor HitColor = FColor::Red);
}