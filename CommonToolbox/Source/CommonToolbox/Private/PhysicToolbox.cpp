// Copyright Tyni Boat, Inc. All Rights Reserved.

#include "PhysicToolbox.h"




bool UPhysicToolbox::CollisionShapeEquals(const FCollisionShape shapeA, const FCollisionShape shapeB)
{
	if (shapeA.ShapeType != shapeB.ShapeType)
		return false;
	return shapeA.GetExtent() == shapeB.GetExtent();
}


FVector UPhysicToolbox::GetKineticEnergy(const FVector velocity, const float mass, const double distanceTraveled)
{
	const FVector momentum = velocity * mass;
	const FVector kinematicEnergy = momentum.GetSafeNormal() * (momentum.SquaredLength() / (2 * mass));
	const FVector force = kinematicEnergy / distanceTraveled;
	return force;
}


void UPhysicToolbox::PostPhysicTrace_internal(const TArray<FHitResult> IncomingHits, TArray<FExpandedHitResult>& outgoingHits, ECollisionChannel Channel,
                                                            FCollisionQueryParams& queryParams, ESurfaceTraceHitType offsetFilter, float PenetrationStep)
{
	outgoingHits.Empty();
	if (IncomingHits.Num() <= 0)
		return;
	const FVector position = IncomingHits[0].TraceStart;
	const FVector direction = (IncomingHits[0].TraceEnd - IncomingHits[0].TraceStart);
	for (int j = 0; j < IncomingHits.Num(); j++)
	{
		auto queryType = IncomingHits[j].Component->GetCollisionResponseToChannel(Channel);
		outgoingHits.Add(FExpandedHitResult(IncomingHits[j], queryType));
		ESurfaceTraceHitType _offset = ESurfaceTraceHitType::NormalHit;
		const float abs_penetrationStep = FMath::Abs(PenetrationStep);
		FVector penetrationIniLocation = IncomingHits[j].ImpactPoint;
		FVector awayOffset = FVector::VectorPlaneProject(IncomingHits[j].ImpactPoint - position, direction.GetSafeNormal());

		if (queryType == ECR_Block && direction.SquaredLength() > 0)
		{
			const auto respParam = FCollisionResponseParams::DefaultResponseParam;
			const auto objParam = FCollisionObjectQueryParams::DefaultObjectQueryParam;

			//Inward chk
			if (awayOffset.Length() > 0.125 && _offset == ESurfaceTraceHitType::NormalHit && (offsetFilter == ESurfaceTraceHitType::InnerHit || offsetFilter == ESurfaceTraceHitType::MAX))
			{
				FHitResult inwardHit;
				FVector pt = IncomingHits[j].ImpactPoint - awayOffset.GetSafeNormal() * 0.125;
				if (IncomingHits[j].GetComponent()->LineTraceComponent(inwardHit, pt, pt + direction, Channel, queryParams, respParam, objParam))
				{
					_offset = ESurfaceTraceHitType::InnerHit;
					penetrationIniLocation = inwardHit.ImpactPoint;
					outgoingHits.Add(FExpandedHitResult(inwardHit, queryType, _offset, inwardHit.Distance));
				}
			}

			//Outward chk
			if (awayOffset.Length() > 0.125 && _offset == ESurfaceTraceHitType::NormalHit && (offsetFilter == ESurfaceTraceHitType::OuterHit || offsetFilter == ESurfaceTraceHitType::MAX))
			{
				FHitResult outwardHit;
				FVector pt = IncomingHits[j].ImpactPoint + awayOffset.GetSafeNormal() * 0.125;
				if (IncomingHits[j].GetComponent()->LineTraceComponent(outwardHit, pt, pt + direction, Channel, queryParams, respParam, objParam))
				{
					_offset = ESurfaceTraceHitType::OuterHit;
					penetrationIniLocation = outwardHit.ImpactPoint;
					outgoingHits.Add(FExpandedHitResult(outwardHit, queryType, _offset, outwardHit.Distance));
				}
			}

			if (abs_penetrationStep > 0)
			{
				int maxSubSurfaceHit = 10;
				FVector penetrationPt = penetrationIniLocation + direction.GetSafeNormal() * abs_penetrationStep;
				FHitResult deepHit;
				while (IncomingHits[j].GetComponent()->SweepComponent(deepHit, penetrationPt, penetrationPt + direction, FQuat::Identity, FCollisionShape::MakeSphere(0.125),
				                                                      queryParams.bTraceComplex))
				{
					if (deepHit.Distance < abs_penetrationStep)
						break;
					if (!deepHit.bStartPenetrating)
					{
						outgoingHits.Add(FExpandedHitResult(deepHit, queryType, _offset, deepHit.Distance + (penetrationPt - penetrationIniLocation).Length()));
					}
					penetrationPt += direction.GetSafeNormal() * FMath::Max(abs_penetrationStep, deepHit.Distance);
					maxSubSurfaceHit--;
					if (maxSubSurfaceHit <= 0)
						break;
				}
			}
		}
	}

	if (outgoingHits.Num() > 1)
	{
		outgoingHits.Sort([position](const FExpandedHitResult& A, const FExpandedHitResult& B)
		{
			return (B.HitResult.ImpactPoint - position).SquaredLength() > (A.HitResult.ImpactPoint - position).SquaredLength();
		});
	}
}

bool UPhysicToolbox::ComponentTraceMulti_internal(UWorld* world, FCollisionShape Shape, ECollisionChannel Channel, TArray<FExpandedHitResult>& outHits, FVector position,
                                                           FVector direction, FQuat rotation, bool traceComplex,
                                                           FCollisionQueryParams& queryParams, ESurfaceTraceHitType offsetFilter, float PenetrationStep)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ComponentTraceCastMulti");
	if (!world)
		return false;
	queryParams.bTraceComplex = traceComplex;
	queryParams.bReturnPhysicalMaterial = true;
	const auto shape = Shape;
	const auto channel = Channel;

	TArray<FHitResult> loopHits;
	const FVector endPoint = position + direction;
	const FCollisionObjectQueryParams objQuery = FCollisionObjectQueryParams::AllObjects;
	if (world->SweepMultiByObjectType(loopHits, position, endPoint, rotation, objQuery, shape, queryParams))
	{
		PostPhysicTrace_internal(loopHits, outHits, channel, queryParams, offsetFilter, PenetrationStep);
	}

	queryParams.ClearIgnoredActors();

	return outHits.Num() > 0;
}


FTraceHandle UPhysicToolbox::AsyncComponentTraceMulti_internal(UWorld* world, FCollisionShape Shape, FVector position, FVector direction, FQuat rotation,
                                                                        FTraceDelegate* Result, bool traceComplex, FCollisionQueryParams& queryParams)
{
	if (!world)
		return {};
	queryParams.bTraceComplex = traceComplex;
	queryParams.bReturnPhysicalMaterial = true;
	const auto shape = Shape;

	const FVector endPoint = position + direction;
	const FCollisionObjectQueryParams objQuery = FCollisionObjectQueryParams::AllObjects;
	const auto handle = world->AsyncSweepByObjectType(EAsyncTraceType::Multi, position, endPoint, rotation, objQuery, shape, queryParams, Result);
	return handle;
}

FVector UPhysicToolbox::OrientationDiffToAngularVelocity(const FQuat initialOrientation, const FQuat targetOrientation)
{
	FVector returnVel = FVector(0);
	FQuat c_targetOrient = targetOrientation;
	c_targetOrient.EnforceShortestArcWith(initialOrientation);
	const FQuat diff = initialOrientation.Inverse() * c_targetOrient;
	FVector axis; float angle;
	diff.ToAxisAndAngle(axis, angle);
	returnVel = axis.GetSafeNormal() * FMath::RadiansToDegrees(angle);
	return returnVel;
}


void UPhysicToolbox::RigidBodyAddForce(const FBodyInstance* BodyInstance, FVector Force, bool bAccelChange)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		if (bAccelChange)
		{
			const float RigidMass = rg->M();
			const Chaos::FVec3 Acceleration = Force * RigidMass;
			rg->AddForce(Acceleration, false);
		}
		else
		{
			rg->AddForce(Force, false);
		}
	}
}

void UPhysicToolbox::RigidBodyAddForceAtPosition(const FBodyInstance* BodyInstance, FVector Position, FVector Force)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(rg);
		const Chaos::FVec3 WorldTorque = Chaos::FVec3::CrossProduct(Position - WorldCOM, Force);
		rg->AddForce(Force, false);
		rg->AddTorque(WorldTorque, false);
	}
}

void UPhysicToolbox::RigidBodyAddTorque(const FBodyInstance* BodyInstance, FVector Torque, bool bAccelChange)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		if (bAccelChange)
		{
			rg->AddTorque(Chaos::FParticleUtilitiesXR::GetWorldInertia(rg) * Torque, false);
		}
		else
		{
			rg->AddTorque(Torque, false);
		}
	}
}

void UPhysicToolbox::RigidBodyAddImpulse(const FBodyInstance* BodyInstance, FVector Impulse, bool bVelChange)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		if (bVelChange)
		{
			rg->SetLinearImpulse(rg->LinearImpulse() + rg->M() * Impulse, false);
		}
		else
		{
			rg->SetLinearImpulse(rg->LinearImpulse() + Impulse, false);
		}
	}
}

void UPhysicToolbox::RigidBodyAddImpulseAtPosition(const FBodyInstance* BodyInstance, FVector Position, FVector Impulse)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(rg);
		const Chaos::FVec3 AngularImpulse = Chaos::FVec3::CrossProduct(Position - WorldCOM, Impulse);
		rg->SetLinearImpulse(rg->LinearImpulse() + Impulse, false);
		rg->SetAngularImpulse(rg->AngularImpulse() + AngularImpulse, false);
	}
}

void UPhysicToolbox::RigidBodyAddAngularImpulseInRadians(const FBodyInstance* BodyInstance, FVector Torque, bool bVelChange)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		if (bVelChange)
		{
			const Chaos::FMatrix33 WorldI = Chaos::FParticleUtilitiesXR::GetWorldInertia(rg);
			rg->SetAngularImpulse(rg->AngularImpulse() + (WorldI * Torque), false);
		}
		else
		{
			rg->SetAngularImpulse(rg->AngularImpulse() + Torque, false);
		}
	}
}

void UPhysicToolbox::RigidBodyAddAngularImpulseInDegrees(const FBodyInstance* BodyInstance, FVector Torque, bool bVelChange)
{
	RigidBodyAddAngularImpulseInRadians(BodyInstance, FMath::DegreesToRadians(Torque), bVelChange);
}

FTransform UPhysicToolbox::GetRigidBodyTransform(const FBodyInstance* BodyInstance)
{
	if (const Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		const Chaos::FRigidTransform3 WorldCOM = Chaos::FParticleUtilitiesGT::GetActorWorldTransform(rg);
		return WorldCOM;
	}
	return FTransform();
}

FVector UPhysicToolbox::GetRigidBodyLinearVelocity(const FBodyInstance* BodyInstance)
{
	if (const Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		return rg->V();
	}
	return FVector::ZeroVector;
}

FVector UPhysicToolbox::GetRigidBodyCOMWorldLocation(const FBodyInstance* BodyInstance)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalRigidBody(BodyInstance))
	{
		if (ensure(RigidHandle->CanTreatAsKinematic()))
		{
			const bool bIsRigid = RigidHandle->CanTreatAsRigid();
			return bIsRigid
				       ? Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle)
				       : static_cast<Chaos::FVec3>(Chaos::FParticleUtilitiesGT::GetActorWorldTransform(RigidHandle).
					       GetTranslation());
		}
	}
	return FVector::ZeroVector;
}

FVector UPhysicToolbox::GetRigidBodyAngularVelocity(const FBodyInstance* BodyInstance)
{
	if (const Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		return rg->W();
	}
	return FVector::ZeroVector;
}

void UPhysicToolbox::RigidBodySetLinearVelocity(const FBodyInstance* BodyInstance, FVector Velocity, bool bAddToCurrent)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		if (bAddToCurrent)
		{
			rg->SetV(rg->V() + Velocity);
		}
		else
		{
			rg->SetV(Velocity);
		}
	}
}

void UPhysicToolbox::RigidBodySetAngularVelocityInRadians(const FBodyInstance* BodyInstance, FVector AngVelocity, bool bAddToCurrent)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		if (bAddToCurrent)
		{
			rg->SetW(rg->W() + AngVelocity);
		}
		else
		{
			rg->SetW(AngVelocity);
		}
	}
}

void UPhysicToolbox::RigidBodySetAngularVelocityInDegrees(const FBodyInstance* BodyInstance, FVector AngVelocity, bool bAddToCurrent)
{
	RigidBodySetAngularVelocityInRadians(BodyInstance, FMath::DegreesToRadians(AngVelocity), bAddToCurrent);
}

void UPhysicToolbox::RigidBodySetWorldLocation(const FBodyInstance* BodyInstance, FVector Location)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		const Chaos::FVec3 P = Location - rg->R().RotateVector(rg->CenterOfMass());
		rg->SetX(P);
	}
}

void UPhysicToolbox::RigidBodySetWorldRotation(const FBodyInstance* BodyInstance, FQuat Rotation)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		const Chaos::FRotation3 Q = Rotation * rg->RotationOfMass().Inverse();
		rg->SetR(Q);
	}
}

void UPhysicToolbox::RigidBodySetWorldLocationAndRotation(const FBodyInstance* BodyInstance, FVector Location, FQuat Rotation)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		const Chaos::FRotation3 Q = Rotation * rg->RotationOfMass().Inverse();
		const Chaos::FVec3 P = Location - Q.RotateVector(rg->CenterOfMass());
		rg->SetR(Q);
		rg->SetX(P);
	}
}

FVector UPhysicToolbox::GetRigidBodyLinearVelocityAtPoint(const FBodyInstance* BodyInstance, FVector Point)
{
	if (Chaos::FRigidBodyHandle_Internal* rg = GetInternalRigidBody(BodyInstance))
	{
		if (ensure(rg->CanTreatAsKinematic()))
		{
			const bool bIsRigid = rg->CanTreatAsRigid();
			const Chaos::FVec3 COM = bIsRigid
				                         ? Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(rg)
				                         : static_cast<Chaos::FVec3>(
					                         Chaos::FParticleUtilitiesGT::GetActorWorldTransform(rg).
					                         GetTranslation());
			const Chaos::FVec3 Diff = Point - COM;
			return rg->V() - Chaos::FVec3::CrossProduct(Diff, rg->W());
		}
	}
	return FVector::ZeroVector;
}

Chaos::FRigidBodyHandle_Internal* UPhysicToolbox::GetInternalRigidBody(const FBodyInstance* BodyInstance)
{
	if(!BodyInstance)
		return nullptr;
	if (const auto Handle = BodyInstance->ActorHandle)
	{
		if (Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI())
		{
			return RigidHandle;
		}
	}
	return nullptr;
}

