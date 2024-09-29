// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "StdContinuousFreeFall.h"

#include "VectorToolbox.h"


UStdContinuousFreeFall::UStdContinuousFreeFall()
{
	this->ModeName = "FreeFall";
	if (this->Priority <= 0)
		this->Priority = 3;
	if (this->BlendSpeed.SquaredLength() <= 0)
		this->BlendSpeed = FVector2D(10, 10);
	if (this->ScanSurfaceVector.SquaredLength() <= 0)
		this->ScanSurfaceVector = FVector(0, 0, -5000);
}

bool UStdContinuousFreeFall::CheckContingentMovement_Implementation(UActorComponent* MoverActorComponent, const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos,
                                                                    const FMomentum& CurrentMomentum,
                                                                    const FVector MoveInput, const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves,
                                                                    const TArray<FTransientMoveInfos>& TransientMoves,
                                                                    TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const
{
	return true;
}

FMechanicProperties UStdContinuousFreeFall::ProcessContingentMovement_Implementation(UActorComponent* MoverActorComponent, FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
                                                                                     const FVector MoveInput,
                                                                                     const FMoverInputPool Inputs, const FTransform SurfacesMovement, const float DeltaTime) const
{
	FMechanicProperties result;
	result.Gravity = GravityVector;
	result.Linear.Acceleration = GravityVector + UVectorToolbox::Project3DVector(MoveInput, GravityVector) * PlanarMoveVelocity;
	result.Linear.DecelerationSpeed = 0;
	result.Linear.TerminalVelocity = TerminalVelocity;
	result.Angular.LookOrientation = UVectorToolbox::Project3DVector(MoveInput, GravityVector).GetSafeNormal() * TurnSpeed;
	return result;
}
