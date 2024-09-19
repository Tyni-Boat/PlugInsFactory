// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "StdContinuousOnGroundMove.h"

#include <Kismet/KismetSystemLibrary.h>

#include "ConversionToolbox.h"
#include "DebugToolbox.h"
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
		if(Surfaces[i].CollisionResponse != ECR_Block)
			continue;
		if(Surfaces[i].OffsetType == ESurfaceTraceHitType::OuterHit)
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
