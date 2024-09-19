// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "StdContinuousOnGroundMove.h"

#include <Kismet/KismetSystemLibrary.h>

#include "DebugToolbox.h"
#include "PhysicToolbox.h"
#include "VectorToolbox.h"


bool UStdContinuousOnGroundMove::CheckContingentMovement_Implementation(const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
                                                                        const FVector MoveInput, const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves, const TArray<FTransientMoveInfos>& TransientMoves,
                                                                        TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const
{
	if(CurrentMomentum.Gravity.SquaredLength() <= 0)
		return false;
	TQueue<FExpandedHitResult> selectedSurfaces;
	const auto shape = CurrentMomentum.GetShape();
	const FVector lowestPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, CurrentMomentum.Gravity);
	const FVector upVector = CurrentMomentum.Gravity.GetSafeNormal();
	UKismetSystemLibrary::DrawDebugPoint(this, lowestPoint, 25, FColor::Blue);
	
	//Collect surfaces meeting conditions and rate them
	for(const auto surface: Surfaces)
	{
		const FVector surfaceHeightVector = (surface.HitResult.ImpactPoint - lowestPoint).ProjectOnToNormal(upVector);
		const float verticalDot = upVector | surfaceHeightVector; 
		// if(verticalDot > 0 && surfaceHeightVector.Length() > MaxStepHeight)
		// 	continue;
		// if(verticalDot < 0 && surfaceHeightVector.Length() > MaxGroundDistance)
		// 	continue;
		UDebugToolbox::DrawDebugCircleOnHit(surface.HitResult, 20, FColor::Green);
		selectedSurfaces.Enqueue(surface);
	}
	//Select the best rated surface
	return !selectedSurfaces.IsEmpty();
}

FMechanicProperties UStdContinuousOnGroundMove::ProcessContingentMovement_Implementation(FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
	const FMoverInputPool Inputs, const float DeltaTime) const
{
	FMechanicProperties result;
	result.Gravity = CurrentMomentum.Gravity;
	result.Linear.Acceleration = UVectorToolbox::Project3DVector(MoveInput, CurrentMomentum.Gravity) * 500;
	result.Linear.DecelerationSpeed = 400;
	result.Linear.TerminalVelocity = FallBackMoveSpeed;
	result.Angular.LookOrientation = UVectorToolbox::Project3DVector(MoveInput, CurrentMomentum.Gravity).GetSafeNormal() * 600;
	return result;
}
