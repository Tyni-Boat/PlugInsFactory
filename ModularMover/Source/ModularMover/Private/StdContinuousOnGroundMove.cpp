// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "StdContinuousOnGroundMove.h"

#include <Kismet/KismetSystemLibrary.h>
#include "Engine/OverlapResult.h"
#include "ConversionToolbox.h"
#include "DebugToolbox.h"
#include "MathToolbox.h"
#include "PhysicToolbox.h"
#include "VectorToolbox.h"


bool UStdContinuousOnGroundMove::CheckContingentMovement_Implementation(const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
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
		//UDebugToolbox::DrawDebugCircleOnHit(Surfaces[i].HitResult, 10, FColor::Orange);
		if (!Surfaces[i].HitResult.Component.IsValid())
			continue;
		if (Surfaces[i].CollisionResponse != ECR_Block)
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
			UDebugToolbox::DrawDebugCircleOnHit(Surfaces[i].HitResult, 10, FColor::Red, .001, 3, true);
			// const FVector finalLoc = CurrentMomentum.Transform.GetLocation() + fromLowPtVector + upVector * 10;
			// const auto channel = ECollisionChannel::ECC_Pawn;
			// TArray<FOverlapResult> testResults;
			// if (!Surfaces[i].HitResult.Component->GetWorld())
			// 	continue;
			// if (Surfaces[i].HitResult.Component->GetWorld()->
			//                 OverlapMultiByChannel(testResults, finalLoc, CurrentMomentum.Transform.GetRotation(), channel, shape, Surfaces[i].QueryParams))
			// {
			// 	continue;
			// }
		}
		const float angle = FMath::Acos(Surfaces[i].HitResult.ImpactNormal | upVector);
		if (angle > FMath::DegreesToRadians(88))
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

FMechanicProperties UStdContinuousOnGroundMove::ProcessContingentMovement_Implementation(FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
                                                                                         const FMoverInputPool Inputs, const float DeltaTime) const
{
	const bool validSurface = CurrentMomentum.Surfaces.IsValidIndex(0);
	const FVector upVector = -CurrentMomentum.Gravity.GetSafeNormal();
	if (!validSurface || upVector.SquaredLength() <= 0)
		return Super::ProcessContingentMovement_Implementation(MoveInfos, CurrentMomentum, MoveInput, Inputs, DeltaTime);

	FMechanicProperties result;
	const auto shape = CurrentMomentum.GetShape();
	const FVector lowestPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, CurrentMomentum.Gravity);
	const FVector4 moveParams = GetMoveParams(Inputs);

	//Linear Move
	result.Gravity = CurrentMomentum.Gravity;
	result.Linear.TerminalVelocity = moveParams.X;
	result.Linear.Acceleration = UVectorToolbox::Project3DVector(MoveInput, UVectorToolbox::VectorCone(CurrentMomentum.Surfaces[0].HitResult.ImpactNormal, upVector, 30)) * moveParams.Y;
	result.Linear.DecelerationSpeed = moveParams.Z;

	//Rotation
	result.Angular.LookOrientation = UVectorToolbox::Project3DVector(MoveInput, upVector).GetSafeNormal() * moveParams.W;

	//Sliding
	const float angle = FMath::Acos(CurrentMomentum.Surfaces[0].HitResult.ImpactNormal | upVector);
	if (angle >= FMath::DegreesToRadians(MinSlopeAngle))
	{
		FMechanicProperties slopeResult;
		FVector slideDirection = FVector::VectorPlaneProject(CurrentMomentum.Surfaces[0].HitResult.ImpactNormal, upVector).GetSafeNormal();
		FVector controlDirection = FVector::VectorPlaneProject(UVectorToolbox::Project3DVector(MoveInput, CurrentMomentum.Surfaces[0].HitResult.ImpactNormal), slideDirection);

		slopeResult.Linear.TerminalVelocity = SlidingMoveParams.X;
		slopeResult.Linear.Acceleration = slideDirection * SlidingMoveParams.Y + controlDirection * SlideControlSpeed;
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
	const FVector snapVector = UVectorToolbox::GetSnapOnSurfaceVector(lowestPoint + upVector * 5, CurrentMomentum.Surfaces[0].HitResult, upVector);
	if (snapVector.SquaredLength() > 0)
	{
		result.Linear.SnapDisplacement = snapVector * FMath::Clamp(DeltaTime * SnapSpeed, 0, 1);
	}

	return result;
}


FVector4 UStdContinuousOnGroundMove::GetMoveParams(const FMoverInputPool& Inputs) const
{
	FVector4 moveParams = FallBackMoveParams;
	for (const auto param : MoveParams)
	{
		bool trigger;
		if (UStructExtension::ReadTriggerInput(Inputs, param.Key, trigger) && trigger)
			moveParams = MoveParams[param.Key];
	}
	return moveParams;
}
