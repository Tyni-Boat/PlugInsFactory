// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "TypesLibrary.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"


#pragma region Enums
#pragma endregion

#pragma region Structs

//------------------------------------------------------------------------------------------------------------------------

FSurface::FSurface()
{
}

FSurface::FSurface(const FBodyInstance* physicBody, FHitResult hit, ESurfaceTraceHitType offsetType)
{
	UpdateHit(physicBody, hit, offsetType);
}

bool FSurface::UpdateTracking(float deltaTime)
{
	FVector linearVelocity = FVector(0);
	FVector angularVelocity = FVector(0);
	bool validSurface = false;

	if (HitResult.Component.IsValid())
	{
		validSurface = true;

		//Linear Part
		linearVelocity = _lastPosition.ContainsNaN() ? FVector(0) : (HitResult.Component->GetSocketLocation(HitResult.BoneName) - _lastPosition) / deltaTime;
		_lastPosition = HitResult.Component->GetSocketLocation(HitResult.BoneName);

		//Angular
		const FQuat targetQuat = HitResult.Component->GetSocketQuaternion(HitResult.BoneName);
		FQuat currentQuat = _lastRotation;

		//Get Angular speed
		if (!_lastRotation.ContainsNaN())
		{
			currentQuat.EnforceShortestArcWith(targetQuat);
			const FQuat quatDiff = targetQuat * currentQuat.Inverse();
			FVector axis;
			float angle;
			quatDiff.ToAxisAndAngle(axis, angle);
			axis.Normalize();
			angularVelocity = axis * FMath::RadiansToDegrees(angle / deltaTime);
		}
		_lastRotation = HitResult.Component->GetSocketQuaternion(HitResult.BoneName);
	}

	LinearVelocity = linearVelocity;
	AngularVelocity = angularVelocity;
	return validSurface;
}

void FSurface::UpdateHit(const FBodyInstance* physicBody, FHitResult hit, ESurfaceTraceHitType offsetType)
{
	if (HitResult.BoneName != hit.BoneName)
	{
		_lastPosition = FVector(NAN);
		_lastRotation = FQuat(NAN,NAN,NAN,NAN);
		LinearVelocity = FVector(0);
		AngularVelocity = FVector(0);
	}
	HitResult = hit;
	SurfaceOffsetType = offsetType;
	bCanStepOn = hit.Component.IsValid()
		             ? (hit.GetComponent()->CanCharacterStepUpOn == ECB_Owner || hit.GetComponent()->CanCharacterStepUpOn == ECB_Yes)
		             : false;
	QueryResponse = hit.Component.IsValid() && physicBody && physicBody->OwnerComponent.IsValid()
		                ? hit.GetComponent()->GetCollisionResponseToComponent(physicBody->OwnerComponent.Get())
		                : ECR_Ignore;
	ObjectType = hit.Component.IsValid()
		             ? UEngineTypes::ConvertToObjectType(hit.GetComponent()->GetCollisionObjectType())
		             : EObjectTypeQuery::ObjectTypeQuery_MAX;
	SurfacePhysicProperties = hit.PhysMaterial.IsValid()
		                          ? FVector(hit.PhysMaterial->Friction, hit.PhysMaterial->Restitution, 0)
		                          : FVector(1, 0, 0);
}

FVector FSurface::ApplyForceAtOnSurface(const FVector point, const FVector force, bool reactionForce) const
{
	if (!HitResult.Component.IsValid())
		return FVector(0);
	if (!HitResult.Component->IsSimulatingPhysics(HitResult.BoneName))
		return FVector(0);
	FVector f = force;
	if (reactionForce)
	{
		f = (f | HitResult.Normal) >= 0 ? FVector(0) : f.ProjectOnToNormal(HitResult.Normal);
	}
	const FVector lastVelocityAt = HitResult.Component->GetPhysicsLinearVelocityAtPoint(point, HitResult.BoneName);
	HitResult.Component->AddForceAtLocation(f, point, HitResult.BoneName);
	return lastVelocityAt;
}

FVector FSurface::GetVelocityAlongNormal(const FVector velocity, const bool useImpactNormal, const bool reactionPlanarOnly) const
{
	if (!HitResult.Component.IsValid())
		return velocity;

	const FVector normal = useImpactNormal ? HitResult.ImpactNormal : HitResult.Normal;
	if (reactionPlanarOnly && (normal | velocity) > 0)
		return velocity;
	return FVector::VectorPlaneProject(velocity, normal);
}

FVector FSurface::GetVelocityAt(const FVector point, const float deltaTime) const
{
	FVector linearPart = LinearVelocity;
	if (HitResult.Normal.SquaredLength() > 0 && linearPart.SquaredLength() > 0)
	{
		linearPart = (linearPart.GetSafeNormal() | HitResult.Normal) >= 0 ? LinearVelocity : FVector::VectorPlaneProject(LinearVelocity, HitResult.Normal);
	}
	if (!HitResult.Component.IsValid())
		return linearPart;

	//Angular part
	const FVector rotationAxis = AngularVelocity.GetSafeNormal();
	const FVector radiusDirection = FVector::VectorPlaneProject(point - HitResult.Component->GetSocketLocation(HitResult.BoneName), rotationAxis).GetSafeNormal();
	FVector tangentialDirection = FVector::CrossProduct(rotationAxis, radiusDirection);
	tangentialDirection.Normalize();
	const double radius = FVector::VectorPlaneProject(point - HitResult.Component->GetSocketLocation(HitResult.BoneName), rotationAxis).Length();
	const double angle = FMath::DegreesToRadians(AngularVelocity.Length());
	const FVector rotVel = radius * angle * tangentialDirection;
	const FVector centripetal = -radiusDirection * (angle * angle) * radius * deltaTime * deltaTime * 1.5; //(1 + deltaTime);

	//Finally
	return linearPart + rotVel + centripetal;
}


//------------------------------------------------------------------------------------------------------------------------


FCommonMoveInfos::FCommonMoveInfos()
{
}

//------------------------------------------------------------------------------------------------------------------------
FContingentMoveInfos::FContingentMoveInfos()
{
}

FContingentMoveInfos::FContingentMoveInfos(UBaseContingentMove* move)
{
	if (!move)
		return;
	BaseInfos.ModeName = move->ModeName;
	BaseInfos.Priority = move->Priority;
	BaseInfos.BlendSpeed = move->BlendSpeed;
}

FTransientMoveInfos::FTransientMoveInfos()
{
}

FTransientMoveInfos::FTransientMoveInfos(UBaseTransientMove* move)
{
	if (!move)
		return;
	BaseInfos.ModeName = move->ModeName;
	BaseInfos.Priority = move->Priority;
	BaseInfos.BlendSpeed = move->BlendSpeed;
	ModeDuration = move->ModeDuration;
}

//------------------------------------------------------------------------------------------------------------------------

bool UStructExtension::ReadVectorInput(const FMoverInputPool InputPool, const FName InputName, FVector& OutVector)
{
	if (!InputPool.InputMap.Contains(InputName))
		return false;
	OutVector = InputPool.InputMap[InputName].Property;
	return true;
}

bool UStructExtension::ReadRotationInput(const FMoverInputPool InputPool, const FName InputName, FRotator& OutRotation)
{
	if (!InputPool.InputMap.Contains(InputName))
		return false;
	OutRotation = FRotator(InputPool.InputMap[InputName].Property.X, InputPool.InputMap[InputName].Property.Y, InputPool.InputMap[InputName].Property.Z);
	return true;
}

bool UStructExtension::ReadValueInput(const FMoverInputPool InputPool, const FName InputName, float& OutValue)
{
	if (!InputPool.InputMap.Contains(InputName))
		return false;
	OutValue = InputPool.InputMap[InputName].Property.X;
	return true;
}

bool UStructExtension::ReadTriggerInput(const FMoverInputPool InputPool, const FName InputName, bool& OutTrigger)
{
	if (!InputPool.InputMap.Contains(InputName))
		return false;
	OutTrigger = InputPool.InputMap[InputName].Property.X > 0;
	return true;
}

void UStructExtension::ApplyForceOnSurfaces(TArray<FSurface>& Surfaces, const FVector point, const FVector force, bool reactionForce, ECollisionResponse channelFilter)
{
	if (Surfaces.Num() <= 0)
		return;
	for (int i = 0; i < Surfaces.Num(); i++)
	{
		if (channelFilter != ECR_MAX && Surfaces[i].QueryResponse != channelFilter)
			continue;
		Surfaces[i].ApplyForceAtOnSurface(point, force, reactionForce);
	}
}

FVector UStructExtension::GetSurfacesVelocityFromNormal(const TArray<FSurface>& Surfaces, const FVector velocity, const bool useImpactNormal, ECollisionResponse channelFilter)
{
	if (Surfaces.Num() <= 0)
		return velocity;
	FVector subSequentVelocity = velocity;
	for (int i = 0; i < Surfaces.Num(); i++)
	{
		if (channelFilter != ECR_MAX && Surfaces[i].QueryResponse != channelFilter)
			continue;
		FVector surVel = Surfaces[i].GetVelocityAlongNormal(subSequentVelocity, useImpactNormal, true);
		const FVector projected = surVel.ProjectOnToNormal(subSequentVelocity.GetSafeNormal());
		const FVector planed = FVector::VectorPlaneProject(surVel, subSequentVelocity.GetSafeNormal());
		if ((projected | subSequentVelocity) > 0)
		{
			if (projected.SquaredLength() > subSequentVelocity.SquaredLength())
				subSequentVelocity = projected;
		}
		else
		{
			subSequentVelocity += projected;
		}
		subSequentVelocity += planed;
	}

	return subSequentVelocity;
}

FVector UStructExtension::GetAverageSurfaceVelocityAt(const TArray<FSurface>& Surfaces, const FVector point, const float deltaTime, ECollisionResponse channelFilter)
{
	if (Surfaces.Num() <= 0)
		return FVector(0);
	FVector cumulated = FVector(0);
	for (int i = 0; i < Surfaces.Num(); i++)
	{
		const auto surface = Surfaces[i];
		if (channelFilter != ECR_MAX && surface.QueryResponse != channelFilter)
			continue;
		const FVector surVel = surface.GetVelocityAt(point, deltaTime);
		if (cumulated.IsNearlyZero())
		{
			cumulated = surVel;
			continue;
		}
		const FVector projected = surVel.ProjectOnToNormal(cumulated.GetSafeNormal());
		const FVector planed = FVector::VectorPlaneProject(surVel, cumulated.GetSafeNormal());
		if ((projected | cumulated) > 0)
		{
			if (projected.SquaredLength() > cumulated.SquaredLength())
				cumulated = projected;
		}
		else
		{
			cumulated += projected;
		}
		cumulated += planed;
	}

	return cumulated;
}

FVector UStructExtension::GetAverageSurfaceAngularSpeed(const TArray<FSurface>& Surfaces, ECollisionResponse channelFilter)
{
	if (Surfaces.Num() <= 0)
		return FVector(0);
	FVector cumulated = FVector(0);
	for (int i = 0; i < Surfaces.Num(); i++)
	{
		const auto surface = Surfaces[i];
		if (channelFilter != ECR_MAX && surface.QueryResponse != channelFilter)
			continue;
		const FVector surRotVel = surface.AngularVelocity;
		FVector axis = surRotVel;
		if (!axis.Normalize())
			continue;
		const double angle = surRotVel.Length();
		cumulated += (axis * angle);
	}

	return cumulated;
}

FVector UStructExtension::GetMaxSurfacePhysicProperties(const TArray<FSurface>& Surfaces, ECollisionResponse channelFilter)
{
	if (Surfaces.Num() <= 0)
		return FVector(0);
	float maxFriction = 0;
	float maxBounce = 0;

	for (int i = 0; i < Surfaces.Num(); i++)
	{
		const auto surface = Surfaces[i];
		if (channelFilter != ECR_MAX && surface.QueryResponse != channelFilter)
			continue;
		if (surface.SurfacePhysicProperties.X > maxFriction)
			maxFriction = surface.SurfacePhysicProperties.X;
		if (surface.SurfacePhysicProperties.Y > maxBounce)
			maxBounce = surface.SurfacePhysicProperties.Y;
	}

	return FVector(maxFriction, maxBounce, 0);
}

#pragma endregion

#pragma region Classes


UBaseMoverMovementMode::UBaseMoverMovementMode()
{
}

bool UBaseMoverMovementMode::IsValid() const
{
	return ModeName != NAME_None && Priority >= 0;
}

//------------------------------------------------------------------------------------------------------------------------

UBaseContingentMove::UBaseContingentMove()
{
}


bool UBaseContingentMove::CheckContingentMovement_Implementation(UActorComponent* MoverActorComponent, const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos,
                                                                 const FMomentum& CurrentMomentum,
                                                                 const FVector MoveInput, const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves,
                                                                 const TArray<FTransientMoveInfos>& TransientMoves,
                                                                 TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const
{
	return false;
}

FMechanicProperties UBaseContingentMove::ProcessContingentMovement_Implementation(UActorComponent* MoverActorComponent, FContingentMoveInfos& MoveInfos,
                                                                                  const FMomentum& CurrentMomentum, const FVector MoveInput,
                                                                                  const FMoverInputPool Inputs, const float DeltaTime) const
{
	auto result = FMechanicProperties();
	result.Gravity = CurrentMomentum.Gravity;
	result.Linear.DecelerationSpeed = 0;
	return result;
}


//------------------------------------------------------------------------------------------------------------------------


UBaseTransientMove::UBaseTransientMove()
{
}


bool UBaseTransientMove::CheckTransientMovement_Implementation(UActorComponent* MoverActorComponent, const TArray<FExpandedHitResult>& Surfaces, FTransientMoveInfos& MoveInfos,
                                                               const FMomentum& CurrentMomentum,
                                                               const FVector MoveInput, const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves,
                                                               const TArray<FTransientMoveInfos>& TransientMoves,
                                                               TMap<FName, FVector>& CustomProperties) const
{
	return false;
}

FMechanicProperties UBaseTransientMove::ProcessTransientMovement_Implementation(UActorComponent* MoverActorComponent, const FMechanicProperties ContingentMove,
                                                                                FTransientMoveInfos& MoveInfos,
                                                                                const FMomentum& CurrentMomentum, const FVector MoveInput,
                                                                                const FMoverInputPool Inputs, const float DeltaTime) const
{
	return ContingentMove;
}

#pragma endregion
