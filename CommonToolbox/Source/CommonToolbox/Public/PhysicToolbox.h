// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolboxTypeLibrary.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "PhysicToolbox.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLBOX_API UPhysicToolbox : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// Compare 2 collision shapes and return true if they are of same type and size.
	static bool CollisionShapeEquals(const FCollisionShape shapeA, const FCollisionShape shapeB);


	// Get the Kinetic Energy of a body of mass (Kg), traveling at velocity. Set distance travelled to get the force.
	UFUNCTION(BlueprintCallable, Category = "Toolbox|Physic")
	static FVector GetKineticEnergy(const FVector velocity, const float mass, const double distanceTraveled = 1);


	/// Check for all collisions at a position and rotation in a direction as overlaps. return true if any collision occurs
	UFUNCTION(BlueprintCallable, Category = "Toolbox|Physic", meta=(AdvancedDisplay = 6))
	static FORCEINLINE bool ComponentTraceMulti(UPrimitiveComponent* Primitive, ECollisionChannel Channel, TArray<FExpandedHitResult>& outHits, FVector position, FVector direction,
	                                            FQuat rotation,
	                                            bool traceComplex = false, ESurfaceTraceHitType offsetFilter = ESurfaceTraceHitType::MAX, float PenetrationCheck = 0)
	{
		if (Primitive)
		{
			FCollisionQueryParams queryParams = FCollisionQueryParams::DefaultQueryParam;
			queryParams.AddIgnoredActor(Primitive->GetOwner());
			return ComponentTraceMulti_internal(Primitive->GetWorld(), Primitive ? Primitive->GetCollisionShape(0) : FCollisionShape::MakeSphere(1), Channel, outHits, position, direction,
			                                    rotation, traceComplex, queryParams, offsetFilter, PenetrationCheck);
		}
		return false;
	}

	// Convert a hit array from hit results to expanded hit results, calculating offset by the way.
	static void PostPhysicTrace_internal(const TArray<FHitResult> IncomingHits, UPARAM(ref) TArray<FExpandedHitResult>& outgoingHits, ECollisionChannel Channel,
	                                          FCollisionQueryParams& queryParams = FCollisionQueryParams::DefaultQueryParam, ESurfaceTraceHitType offsetFilter = ESurfaceTraceHitType::MAX,
	                                          float PenetrationStep = 0);

	static bool ComponentTraceMulti_internal(UWorld* world, FCollisionShape Shape, ECollisionChannel Channel, TArray<FExpandedHitResult>& outHits, FVector position, FVector direction,
	                                         FQuat rotation,
	                                         bool traceComplex = false, FCollisionQueryParams& queryParams = FCollisionQueryParams::DefaultQueryParam,
	                                         ESurfaceTraceHitType offsetFilter = ESurfaceTraceHitType::MAX, float PenetrationStep = 0);

	static FTraceHandle AsyncComponentTraceMulti_internal(UWorld* world, FCollisionShape Shape, FVector position, FVector direction, FQuat rotation,
	                                                      FTraceDelegate* Result, bool traceComplex = false
	                                                      , FCollisionQueryParams& queryParams = FCollisionQueryParams::DefaultQueryParam);


	// Add force to an async physic rigidBody
	static void RigidBodyAddForce(const FBodyInstance* BodyInstance, FVector Force, bool bAccelChange);

	// Add force to an async physic rigidBody at a position
	static void RigidBodyAddForceAtPosition(const FBodyInstance* BodyInstance, FVector Position, FVector Force);

	// Add Impulse force to an async physic rigidBody
	static void RigidBodyAddImpulse(const FBodyInstance* BodyInstance, FVector Impulse, bool bVelChange);
	
	// Add Impulse force to an async physic rigidBody at a position
	static void RigidBodyAddImpulseAtPosition(const FBodyInstance* BodyInstance, FVector Position, FVector Impulse);

	
	// Add Torque to an async physic rigidBody
	static void RigidBodyAddTorque(const FBodyInstance* BodyInstance, FVector Torque, bool bAccelChange);

	// Add angular impulse to an async physic rigidBody (Rad/s)
	static void RigidBodyAddAngularImpulseInRadians(const FBodyInstance* BodyInstance, FVector Torque, bool bVelChange);

	// Add angular impulse to an async physic rigidBody (Deg/s)
	static void RigidBodyAddAngularImpulseInDegrees(const FBodyInstance* BodyInstance, FVector Torque, bool bVelChange);


	// Get the transform of an async physic rigidBody
	static FTransform GetRigidBodyTransform(const FBodyInstance* BodyInstance);

	// Get the linear velocity of an async physic rigidBody
	static FVector GetRigidBodyLinearVelocity(const FBodyInstance* BodyInstance);
	
	// Get the linear velocity of an async physic rigidBody at a point
	static FVector GetRigidBodyLinearVelocityAtPoint(const FBodyInstance* BodyInstance, FVector Point);

	// Get the angular velocity of an async physic rigidBody (Rad/s)
	static FVector GetRigidBodyAngularVelocity(const FBodyInstance* BodyInstance);
	
	// Get the world space Center Of Mass of an async physic rigidBody
	static FVector GetRigidBodyCOMWorldLocation(const FBodyInstance* BodyInstance);


	// Set the linear velocity of an async physic rigidBody
	static void RigidBodySetLinearVelocity(const FBodyInstance* BodyInstance, FVector Velocity, bool bAddToCurrent);

	// Set the angular velocity of an async physic rigidBody
	static void RigidBodySetAngularVelocityInRadians(const FBodyInstance* BodyInstance, FVector AngVelocity, bool bAddToCurrent);

	// Set the angular velocity of an async physic rigidBody
	static void RigidBodySetAngularVelocityInDegrees(const FBodyInstance* BodyInstance, FVector AngVelocity, bool bAddToCurrent);


	
	// Set the location of an async physic rigidBody
	static void RigidBodySetWorldLocation(const FBodyInstance* BodyInstance, FVector Location);

	// Set the Rotation of an async physic rigidBody
	static void RigidBodySetWorldRotation(const FBodyInstance* BodyInstance, FQuat Rotation);

	// Set both the Location and Rotation of an async physic rigidBody
	static void RigidBodySetWorldLocationAndRotation(const FBodyInstance* BodyInstance, FVector Location, FQuat Rotation);


	static Chaos::FRigidBodyHandle_Internal* GetInternalRigidBody(const FBodyInstance* BodyInstance);
	
};
