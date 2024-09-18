// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "StdContinuousOnGroundMove.h"

#include "VectorToolbox.h"


bool UStdContinuousOnGroundMove::CheckContingentMovement_Implementation(const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos, const FMomentum CurrentMomentum,
                                                                        const FVector MoveInput, const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves, const TArray<FTransientMoveInfos>& TransientMoves,
                                                                        TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const
{
	TQueue<FExpandedHitResult*> selectedSurfaces;
	//Collect surfaces meeting conditions and rate them
	for(const auto surface: Surfaces)
	{
		
	}
	//Select the best rated surface
	return Surfaces.Num() > 0;
}

FMechanicProperties UStdContinuousOnGroundMove::ProcessContingentMovement_Implementation(FContingentMoveInfos& MoveInfos, const FMomentum CurrentMomentum, const FVector MoveInput,
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
