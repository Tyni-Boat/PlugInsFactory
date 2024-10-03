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

bool UStdContinuousFreeFall::CheckContinuousMovement_Implementation(UActorComponent* MoverActorComponent, const TArray<FMoverHitResult>& Surfaces, FContinuousMoveInfos& MoveInfos,
                                                                    const FMomentum& CurrentMomentum,
                                                                    const FVector MoveInput, const FMoverInputPool Inputs, const TArray<FContinuousMoveInfos>& ContinuousMoves,
                                                                    const TArray<FTemporaryMoveInfos>& TemporaryMoves,
                                                                    TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const
{
	return true;
}

FMechanicProperties UStdContinuousFreeFall::ProcessContinuousMovement_Implementation(UActorComponent* MoverActorComponent, FContinuousMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
                                                                                     const FVector MoveInput,
                                                                                     const FMoverInputPool Inputs, const FTransform SurfacesMovement, const float DeltaTime) const
{
	const FVector fallingVelocity = CurrentMomentum.LinearVelocity.ProjectOnToNormal(GravityVector.GetSafeNormal());
	const FVector steeringVelocity = FVector::VectorPlaneProject(CurrentMomentum.LinearVelocity, GravityVector.GetSafeNormal());
	const FVector steeringVector = UVectorToolbox::Project3DVector(MoveInput, GravityVector) * PlanarMoveVelocity;


	FMechanicProperties result;
	result.Gravity = GravityVector;
	result.Linear.Acceleration = (fallingVelocity.Length() < TerminalVelocity ? fallingVelocity + GravityVector * DeltaTime : fallingVelocity)
		+ (UVectorToolbox::AddVectorUntilLimit(steeringVelocity, steeringVector * DeltaTime, PlanarMoveVelocity));
	result.Angular.LookOrientation = UVectorToolbox::Project3DVector(MoveInput, GravityVector).GetSafeNormal() * TurnSpeed;
	return result;
}
