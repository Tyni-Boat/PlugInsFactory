// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "StdContinuousInWaterMove.h"


#include <Kismet/KismetSystemLibrary.h>
#include "Engine/OverlapResult.h"
#include "ConversionToolbox.h"
#include "DebugToolbox.h"
#include "MathToolbox.h"
#include "PhysicToolbox.h"
#include "VectorToolbox.h"


UStdContinuousInWaterMove::UStdContinuousInWaterMove()
{
	this->ModeName = "InWater";
	this->Priority = 4;
	this->BlendSpeed = FVector2D(10, 10);
	this->ScanSurfaceVector = FVector(0, 0, 5000);
	this->ScanSurfaceOffset = 0;
}

bool UStdContinuousInWaterMove::CheckContingentMovement_Implementation(UActorComponent* MoverActorComponent, const TArray<FExpandedHitResult>& Surfaces,
                                                                       FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
                                                                       const FVector MoveInput, const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves,
                                                                       const TArray<FTransientMoveInfos>& TransientMoves,
                                                                       TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const
{
	bool isActive = MoveInfos.BaseInfos.bIsActiveMode;
	if (CurrentMomentum.Gravity.SquaredLength() <= 0)
		return isActive;
	const auto shape = CurrentMomentum.GetShape();
	const FVector lowestPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, CurrentMomentum.Gravity);
	const FVector upVector = -CurrentMomentum.Gravity.GetSafeNormal();

	//Selection
	int bestIndex = -1;
	float bestDistance = 0;

	//Collect surfaces meeting conditions
	for (int i = 0; i < Surfaces.Num(); i++)
	{
		if (!Surfaces[i].HitResult.Component.IsValid())
			continue;
		if (Surfaces[i].CollisionResponse != ECR_Overlap)
			continue;
		if (Surfaces[i].HitResult.Component->GetCollisionObjectType() != WaterChannel)
			continue;
		FVector waterSurface = Surfaces[i].HitResult.ImpactPoint;
		FHitResult waterHitClone = Surfaces[i].HitResult;
		if (UStructExtension::ReadVectorInput(Inputs, WaterSurfaceLocation, waterSurface))
		{
			waterHitClone.ImpactPoint = waterSurface;
		}
		const FVector fromCenterVector = waterHitClone.ImpactPoint - CurrentMomentum.Transform.GetLocation();
		const FVector fromLowPtVector = waterHitClone.ImpactPoint - lowestPoint;
		const FVector surfaceCenterHeightVector = fromCenterVector.ProjectOnToNormal(upVector);
		const FVector surfaceLowHeightVector = fromLowPtVector.ProjectOnToNormal(upVector);
		const float centerDot = upVector | fromCenterVector;
		const float verticalDot = upVector | fromLowPtVector;
		if (verticalDot < 0)
			continue;
		if (verticalDot > 0)
		{
			if (!MoveInfos.BaseInfos.bIsActiveMode && surfaceLowHeightVector.Length() < FloatingHeight)
				continue;
			if (MoveInfos.BaseInfos.bIsActiveMode && surfaceLowHeightVector.Length() < OutWaterImmersion)
			{
				isActive = false;
				continue;
			}
		}
		if (surfaceCenterHeightVector.Length() <= bestDistance)
		{
			if (!FMath::IsNearlyEqual(surfaceCenterHeightVector.Length(), bestDistance, 1))
				continue;
		}
		bestDistance = surfaceCenterHeightVector.Length();
		bestIndex = i;
	}

	//Surface flag
	SurfacesFlag = Surfaces.IsValidIndex(bestIndex) ? UConversionToolbox::IndexToFlag(bestIndex) : 0;
	//Custom Props
	if (CustomProperties.Contains("WaterImmersion")) CustomProperties["WaterImmersion"] = FVector(bestDistance, 0, 0);
	else CustomProperties.Add("WaterImmersion", FVector(bestDistance, 0, 0));

	if (!isActive)
		isActive = Surfaces.IsValidIndex(bestIndex);

	return isActive;
}

FMechanicProperties UStdContinuousInWaterMove::ProcessContingentMovement_Implementation(UActorComponent* MoverActorComponent, FContingentMoveInfos& MoveInfos,
                                                                                        const FMomentum& CurrentMomentum, const FVector MoveInput,
                                                                                        const FMoverInputPool Inputs, const FTransform SurfacesMovement, const float DeltaTime) const
{
	const bool validSurface = CurrentMomentum.Surfaces.IsValidIndex(0);
	const FVector upVector = -CurrentMomentum.Gravity.GetSafeNormal();
	if (!validSurface || upVector.SquaredLength() <= 0)
		return Super::ProcessContingentMovement_Implementation(MoverActorComponent, MoveInfos, CurrentMomentum, MoveInput, Inputs, SurfacesMovement, DeltaTime);

	FMechanicProperties result;
	const auto shape = CurrentMomentum.GetShape();
	FVector waterSurface = CurrentMomentum.Surfaces[0].HitResult.ImpactPoint;
	FHitResult waterHitClone = CurrentMomentum.Surfaces[0].HitResult;
	if (UStructExtension::ReadVectorInput(Inputs, WaterSurfaceLocation, waterSurface))
	{
		waterHitClone.ImpactPoint = waterSurface;
	}
	const FVector verticalVelocity = CurrentMomentum.LinearVelocity.ProjectOnToNormal(upVector);
	const FVector lowestPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, CurrentMomentum.Gravity);
	const FVector highestPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, CurrentMomentum.Transform, -CurrentMomentum.Gravity);
	const FVector4 moveParams = GetMoveParams(Inputs);
	const float immersion = (waterHitClone.ImpactPoint - lowestPoint).ProjectOnToNormal(upVector).Length();
	const FVector impactNormal = waterHitClone.ImpactNormal;
	const float viscosity = CurrentMomentum.Surfaces[0].SurfacePhysicProperties.X;
	const FVector verticalMoveInp = MoveInput.ProjectOnToNormal(upVector);
	const FVector horizontalMoveInp = FVector::VectorPlaneProject(MoveInput, upVector);

	//Linear Move
	result.Gravity = CurrentMomentum.Gravity;
	result.Linear.TerminalVelocity = moveParams.X;
	result.Linear.Acceleration = (horizontalMoveInp + (immersion >= FloatingHeight && (verticalMoveInp | upVector) > 0 ? FVector(0) : verticalMoveInp)) * moveParams.Y * viscosity;
	result.Linear.DecelerationSpeed = moveParams.Z * viscosity;
	result.SurfacesMovement = SurfacesMovement;

	//Rotation
	result.Angular.LookOrientation = UVectorToolbox::Project3DVector(MoveInput, upVector).GetSafeNormal() * moveParams.W;

	//Buoyancy
	FVector buoyancyVector = UVectorToolbox::GetSnapOnSurfaceVector(lowestPoint + upVector * FloatingHeight, waterHitClone, upVector);
	// if (buoyancyVector.Length() > 5)
	// {
	// 	buoyancyVector *= ArchimedForceScale;
	// 	if ((verticalVelocity | -upVector) < 0)
	// 	{
	// 		const FVector kinetic = -UPhysicToolbox::GetKineticEnergy(verticalVelocity, CurrentMomentum.Mass, FMath::Abs(immersion - FloatingHeight));
	// 		const float controllerLenght = (lowestPoint - highestPoint).Length();
	// 		buoyancyVector += (kinetic / CurrentMomentum.Mass) * FMath::Clamp((immersion - OutWaterImmersion) / (controllerLenght - FloatingHeight), 0, 1);
	// 	}
	// 	result.Linear.Acceleration += buoyancyVector;
	// }
	result.SurfacesMovement.SetTranslation(buoyancyVector * ArchimedForceScale + SurfacesMovement.GetTranslation());

	return result;
}


FVector4 UStdContinuousInWaterMove::GetMoveParams(const FMoverInputPool& Inputs) const
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
