// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "StdContinuousOnGroundMove.h"

#include <Kismet/KismetSystemLibrary.h>
#include "Engine/OverlapResult.h"
#include "ConversionToolbox.h"
#include "DebugToolbox.h"
#include "MathToolbox.h"
#include "PhysicToolbox.h"
#include "VectorToolbox.h"


UStdContinuousOnGroundMove::UStdContinuousOnGroundMove()
{
	this->ModeName = "OnGround";
	this->Priority = 3;
	this->BlendSpeed = FVector2D(10, 10);
	this->ScanSurfaceVector = FVector(0, 0, -500);
	this->ScanSurfaceOffset = 150;
}

bool UStdContinuousOnGroundMove::CheckContingentMovement_Implementation(UActorComponent* MoverActorComponent, const TArray<FExpandedHitResult>& Surfaces,
                                                                        FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
                                                                        const FVector MoveInput, const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves,
                                                                        const TArray<FTransientMoveInfos>& TransientMoves,
                                                                        TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const
{
	if (CurrentMomentum.Gravity.SquaredLength() <= 0)
		return false;
	const auto shape = CurrentMomentum.GetShape();
	const FVector lowestPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, CurrentMomentum.Gravity);
	const FVector upVector = -CurrentMomentum.Gravity.GetSafeNormal();

	//Selection
	int bestIndex = -1;
	float bestDistance = TNumericLimits<float>::Max();
	float bestAngle = FMath::DegreesToRadians(90);

	//Collect surfaces meeting conditions
	for (int i = 0; i < Surfaces.Num(); i++)
	{
		if (!Surfaces[i].HitResult.Component.IsValid())
			continue;
		if (Surfaces[i].CollisionResponse != ECR_Block)
			continue;
		if (Surfaces[i].HitResult.Component->GetCollisionObjectType() != GroundChannel)
			continue;
		const FVector fromCenterVector = Surfaces[i].HitResult.ImpactPoint - CurrentMomentum.Transform.GetLocation();
		const FVector fromLowPtVector = Surfaces[i].HitResult.ImpactPoint - lowestPoint;
		const FVector surfaceCenterHeightVector = fromCenterVector.ProjectOnToNormal(upVector);
		const FVector surfaceLowHeightVector = fromLowPtVector.ProjectOnToNormal(upVector);
		const float centerDot = upVector | fromCenterVector;
		const float verticalDot = upVector | fromLowPtVector;
		if (verticalDot > 0 && surfaceLowHeightVector.Length() > MaxStepHeight)
			continue;
		if (verticalDot < 0 && surfaceLowHeightVector.Length() > MaxGroundDistance)
			continue;
		if (Surfaces[i].OffsetType == ESurfaceTraceHitType::OuterHit && verticalDot > 0)
		{
			if (!MoveInfos.BaseInfos.bIsActiveMode)
				continue;
			if (!UVectorToolbox::IsVectorCone(Surfaces[i].HitResult.ImpactNormal, Surfaces[i].HitResult.Normal, 10))
				continue;
		}
		const float angle = FMath::Acos(Surfaces[i].HitResult.ImpactNormal | upVector);
		if (angle > FMath::DegreesToRadians(MaxSurfaceAngle))
			continue;
		if (surfaceCenterHeightVector.Length() >= bestDistance)
		{
			if (!FMath::IsNearlyEqual(surfaceCenterHeightVector.Length(), bestDistance, 1))
				continue;
			if (angle >= bestAngle)
				continue;
		}
		bestDistance = surfaceCenterHeightVector.Length();
		bestAngle = angle;
		bestIndex = i;
	}

	//Surface flag
	SurfacesFlag = Surfaces.IsValidIndex(bestIndex) ? UConversionToolbox::IndexToFlag(bestIndex) : 0;
	//Custom Props
	if (CustomProperties.Contains("GroundDistance")) CustomProperties["GroundDistance"] = FVector(bestDistance, 0, 0);
	else CustomProperties.Add("GroundDistance", FVector(bestDistance, 0, 0));

	return Surfaces.IsValidIndex(bestIndex);
}

FMechanicProperties UStdContinuousOnGroundMove::ProcessContingentMovement_Implementation(UActorComponent* MoverActorComponent, FContingentMoveInfos& MoveInfos,
                                                                                         const FMomentum& CurrentMomentum, const FVector MoveInput,
                                                                                         const FMoverInputPool Inputs, const FTransform SurfacesMovement, const float DeltaTime) const
{
	const bool validSurface = CurrentMomentum.Surfaces.IsValidIndex(0);
	const FVector upVector = -CurrentMomentum.Gravity.GetSafeNormal();
	if (!validSurface || upVector.SquaredLength() <= 0)
		return Super::ProcessContingentMovement_Implementation(MoverActorComponent, MoveInfos, CurrentMomentum, MoveInput, Inputs, SurfacesMovement, DeltaTime);

	FMechanicProperties result;
	const auto shape = CurrentMomentum.GetShape();
	const FVector lowestPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, CurrentMomentum.Gravity);
	const FVector4 moveParams = GetMoveParams(Inputs);
	const float friction = CurrentMomentum.Surfaces[0].SurfacePhysicProperties.X;
	const FVector impactNormal = CurrentMomentum.Surfaces[0].HitResult.ImpactNormal;

	//Linear Move
	result.Gravity = CurrentMomentum.Gravity;
	result.Linear.TerminalVelocity = moveParams.X;
	result.Linear.Acceleration = UVectorToolbox::Project3DVector(MoveInput, UVectorToolbox::VectorCone(impactNormal, upVector, 35)) * moveParams.Y * friction;
	result.Linear.DecelerationSpeed = moveParams.Z * friction;
	result.SurfacesMovement = SurfacesMovement;

	//Rotation
	result.Angular.LookOrientation = UVectorToolbox::Project3DVector(MoveInput, upVector).GetSafeNormal() * moveParams.W;

	//Sliding
	const float angle = FMath::Acos(impactNormal | upVector);
	if (angle >= FMath::DegreesToRadians(MinSlopeAngle))
	{
		FMechanicProperties slopeResult;
		FVector slideDirection = FVector::VectorPlaneProject(impactNormal, upVector).GetSafeNormal();
		FVector controlDirection = FVector::VectorPlaneProject(UVectorToolbox::Project3DVector(MoveInput, impactNormal), slideDirection);

		slopeResult.Linear.TerminalVelocity = SlidingMoveParams.X;
		slopeResult.Linear.Acceleration = FVector::VectorPlaneProject(-upVector, impactNormal).GetSafeNormal() * SlidingMoveParams.Y + controlDirection * SlideControlSpeed;
		slopeResult.Linear.DecelerationSpeed = SlidingMoveParams.Z;
		slopeResult.Angular.LookOrientation = SlidingMoveParams.W > 0 ? slideDirection * SlidingMoveParams.W : result.Angular.LookOrientation;

		const float slopeMoveAmount = FMath::GetMappedRangeValueClamped(TRange<float>(MinSlopeAngle, MaxSlopeAngle), TRange<float>(0, 1), FMath::RadiansToDegrees(angle));
		const float slideMoveAmount = (result.Linear.Acceleration.GetSafeNormal() | slideDirection);
		const float accScale = 1 - FMath::GetMappedRangeValueClamped(TRange<float>(-1, 1), TRange<float>(0, 1), slideMoveAmount);
		result.Linear.Acceleration -= result.Linear.Acceleration * accScale * slopeMoveAmount;
		result.Linear.TerminalVelocity -= result.Linear.TerminalVelocity * accScale * slopeMoveAmount;
		if (angle >= FMath::DegreesToRadians(MaxSlopeAngle))
		{
			result.Linear = slopeResult.Linear;
			result.Angular = slopeResult.Angular;
		}
	}

	//Snapping
	const FVector snapVector = UVectorToolbox::GetSnapOnSurfaceVector(lowestPoint - upVector, CurrentMomentum.Surfaces[0].HitResult, upVector);
	if (snapVector.SquaredLength() > 0)
	{
		if ((snapVector | upVector) >= 0 || (bDownSnapping && (snapVector | upVector) < 0))
		{
			result.Linear.SnapDisplacement = snapVector * FMath::Clamp(DeltaTime * SnapSpeed, 0, 1);
		}
		else
		{
			result.Linear.Acceleration += result.Gravity;
		}
	}

	return result;
}


FVector4 UStdContinuousOnGroundMove::GetMoveParams(const FMoverInputPool& Inputs) const
{
	FVector4 moveParams = FallBackMoveParams;
	for (const auto param : MoveParams)
	{
		float value;
		if (UStructExtension::ReadValueInput(Inputs, param.Key, value))
			moveParams = MoveParams[param.Key] * value;
	}
	return moveParams;
}
