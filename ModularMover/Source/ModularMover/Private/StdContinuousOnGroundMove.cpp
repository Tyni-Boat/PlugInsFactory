// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "StdContinuousOnGroundMove.h"

#include <Kismet/KismetSystemLibrary.h>

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
		UDebugToolbox::DrawDebugCircleOnHit(Surfaces[i].HitResult, 10, FColor::Orange);
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
			const FVector finalLoc = CurrentMomentum.Transform.GetLocation() + fromLowPtVector;
			const auto channel = ECollisionChannel::ECC_Pawn;
			if (Surfaces[i].HitResult.Component.IsValid()
				&& Surfaces[i].HitResult.Component->GetWorld()->OverlapBlockingTestByChannel(finalLoc, CurrentMomentum.Transform.GetRotation(), channel, shape))
			{
				UDebugToolbox::DrawDebugCircleOnHit(Surfaces[i].HitResult, 10, FColor::Red, 0, 3);
				continue;
			}
		}
		const float angle = FMath::Acos(Surfaces[i].HitResult.ImpactNormal | upVector);
		if (angle > FMath::DegreesToRadians(88))
			continue;
		if (surfaceCenterHeightVector.Length() >= bestDistance)
		{
			if (surfaceCenterHeightVector.Length() != bestDistance)
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
	return Surfaces.IsValidIndex(bestIndex);


	// FVector gravityDirection = CurrentMomentum.Gravity.GetSafeNormal();
	// FVector location = CurrentMomentum.Transform.GetLocation();
	// if (!gravityDirection.Normalize())
	// 	gravityDirection = FVector::DownVector;
	// const auto shape = CurrentMomentum.GetShape();
	// const FVector lowestPt = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, gravityDirection);
	// const FVector velocity = CurrentMomentum.LinearVelocity;
	//
	// //Find the best surface
	// int surfaceIndex = -1;
	// float closestSurface = TNumericLimits<float>::Max();
	// float closestSurface_low = TNumericLimits<float>::Max();
	// float closestCheckSurface = TNumericLimits<float>::Max();
	//
	// // Default on bad angles
	// int badAngleIndex = -1;
	// float closestBadAngle = TNumericLimits<float>::Max();
	//
	// for (int i = 0; i < Surfaces.Num(); i++)
	// {
	// 	const auto surface = Surfaces[i];
	// 	//Valid surface verification
	// 	if (!surface.HitResult.Component.IsValid())
	// 		continue;
	//
	// 	if (surface.CollisionResponse != ECR_Block)
	// 		continue;
	//
	// 	if (surface.HitResult.Component->GetCollisionObjectType() != GroundChannel)
	// 		continue;
	//
	// 	//Only surfaces we can step on
	// 	bool canStepOn = surface.HitResult.Component.IsValid()
	// 		                 ? (surface.HitResult.GetComponent()->CanCharacterStepUpOn == ECB_Owner || surface.HitResult.GetComponent()->CanCharacterStepUpOn == ECB_Yes)
	// 		                 : false;
	// 	if (!canStepOn)
	// 		continue;
	//
	// 	//Above surface verification
	// 	const FVector fromCenter = (surface.HitResult.ImpactPoint - location).GetSafeNormal();
	// 	if ((fromCenter | gravityDirection) <= 0)
	// 		continue;
	//
	// 	const FVector centerHeightVector = (surface.HitResult.ImpactPoint - location).ProjectOnToNormal(-gravityDirection);
	// 	const FVector heightVector = (surface.HitResult.ImpactPoint - lowestPt).ProjectOnToNormal(-gravityDirection);
	// 	const float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(MoveInfos.BaseInfos.bIsActiveMode ? surface.HitResult.ImpactNormal : surface.HitResult.Normal,
	// 	                                                                            -gravityDirection)));
	// 	const FVector farAwayVector = FVector::VectorPlaneProject(surface.HitResult.ImpactPoint - location, gravityDirection);
	// 	const FVector shapePtInDir = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, farAwayVector);
	// 	const FVector inShapeDir = shapePtInDir - location;
	// 	if (UMathToolbox::CheckInRange(FVector2D(0, 89), angle) && closestCheckSurface > heightVector.Length()
	// 		&& (inShapeDir.SquaredLength() <= 0 || (inShapeDir.SquaredLength() > 0 && farAwayVector.Length() < inShapeDir.Length() * 0.75)))
	// 	{
	// 		closestCheckSurface = heightVector.Length();
	// 	}
	//
	// 	//Step height verification
	// 	if (heightVector.Length() > (MaxStepHeight + ((heightVector | gravityDirection) > 0 ? 10 : 0)))
	// 		continue;
	//
	// 	// Avoid too far down surfaces on first detection
	// 	if (!MoveInfos.BaseInfos.bIsActiveMode && heightVector.Length() > FLOATING_HEIGHT && (heightVector | gravityDirection) > 0)
	// 		continue;
	//
	// 	//Angle verification
	// 	if (!UMathToolbox::CheckInRange(FVector2D(0, 89), angle))
	// 	{
	// 		const float badDistance = heightVector.Length() * ((surface.HitResult.ImpactPoint - lowestPt).GetSafeNormal() | gravityDirection);
	// 		if (badDistance >= closestBadAngle)
	// 		{
	// 			continue;
	// 		}
	// 		if (!MoveInfos.BaseInfos.bIsActiveMode && (surface.HitResult.Normal | surface.HitResult.ImpactNormal) < 0.9)
	// 			continue;
	//
	// 		badAngleIndex = i;
	// 		closestBadAngle = badDistance;
	// 		continue;
	// 	}
	//
	// 	//Avoid far distances when not active
	// 	if (!MoveInfos.BaseInfos.bIsActiveMode && inShapeDir.SquaredLength() > 0 && farAwayVector.Length() >= inShapeDir.Length() * 0.75)
	// 		continue;
	//
	// 	const float distance_low = heightVector.Length() * ((surface.HitResult.ImpactPoint - lowestPt).GetSafeNormal() | gravityDirection);
	// 	const float distance = centerHeightVector.Length();
	// 	if (distance >= closestSurface)
	// 	{
	// 		continue;
	// 	}
	//
	// 	//How far from center is the surface point
	// 	if (inShapeDir.SquaredLength() > 0 && inShapeDir.Length() <= farAwayVector.Length() && (heightVector | gravityDirection) < 0)
	// 	{
	// 		// Check if the step is safe
	// 		if ((closestSurface - distance) >= FLOATING_HEIGHT)
	// 		{
	// 			const FVector virtualSnap = UVectorToolbox::GetSnapOnSurfaceVector(lowestPt, surface.HitResult, gravityDirection);
	// 			const FVector offset = farAwayVector.GetSafeNormal() * 1 + virtualSnap + virtualSnap.GetSafeNormal() * FLOATING_HEIGHT;
	// 			const auto channel = ECollisionChannel::ECC_Pawn;
	// 			if (surface.HitResult.Component->GetWorld()->OverlapBlockingTestByChannel(location + offset, CurrentMomentum.Transform.GetRotation(), channel, shape))
	// 			{
	// 				continue;
	// 			}
	// 		}
	// 	}
	//
	// 	closestSurface = distance;
	// 	closestSurface_low = distance_low;
	// 	surfaceIndex = i;
	// }
	//
	// const FVector cProp = FVector(closestSurface_low < closestCheckSurface ? closestSurface_low : closestCheckSurface, 0, 0);
	// if (CustomProperties.Contains("GroundDistance")) CustomProperties["GroundDistance"] = cProp;
	// else CustomProperties.Add("GroundDistance", cProp);
	//
	// //Compute the flag
	// int flag = UConversionToolbox::BoolArrayToFlag(UConversionToolbox::IndexesToBoolArray(TArray<int>{surfaceIndex, badAngleIndex}));
	// SurfacesFlag = flag;
	//
	// return flag > 0;
}

FMechanicProperties UStdContinuousOnGroundMove::ProcessContingentMovement_Implementation(FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
                                                                                         const FMoverInputPool Inputs, const float DeltaTime) const
{
	const bool validSurface = CurrentMomentum.Surfaces.IsValidIndex(0);
	FMechanicProperties result;
	result.Gravity = CurrentMomentum.Gravity;
	result.Linear.Acceleration = UVectorToolbox::Project3DVector(MoveInput, CurrentMomentum.Gravity) * FallBackMoveSpeed * 10;
	result.Linear.DecelerationSpeed = FallBackMoveSpeed * 4;
	result.Linear.TerminalVelocity = FallBackMoveSpeed;
	if (validSurface)
	{
		const auto shape = CurrentMomentum.GetShape();
		const FVector lowestPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, CurrentMomentum.Gravity);
		const FVector snapVector = UVectorToolbox::GetSnapOnSurfaceVector(lowestPoint, CurrentMomentum.Surfaces[0].HitResult, CurrentMomentum.Gravity.GetSafeNormal());
		UKismetSystemLibrary::DrawDebugArrow(CurrentMomentum.Surfaces[0].HitResult.GetComponent(), lowestPoint, lowestPoint + snapVector, 25, FColor::Red);

		result.Linear.SnapDisplacement = snapVector * FMath::Clamp(DeltaTime * 25, 0, 1);
	}
	result.Angular.LookOrientation = UVectorToolbox::Project3DVector(MoveInput, CurrentMomentum.Gravity).GetSafeNormal() * 600;
	return result;
}
