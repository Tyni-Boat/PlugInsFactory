// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CollisionShape.h"
#include "CommonToolboxBPLibrary.h"
#include "Engine/DataAsset.h"
#include "UObject/ObjectMacros.h"
#include "WorldCollision.h"
#include "TypesLibrary.generated.h"


#pragma region Enums

#pragma endregion

#pragma region Structs


USTRUCT(BlueprintType)
struct MODULARMOVER_API FSurface
{
	GENERATED_BODY()

	FSurface();

	FSurface(FBodyInstance physicBody, FHitResult hit, ESurfaceTraceHitType offsetType = ESurfaceTraceHitType::NormalHit);

	// the hit ray from witch the surface were detected. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface")
	FHitResult HitResult;

	// The query response.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface")
	TEnumAsByte<ECollisionResponse> QueryResponse = ECollisionResponse::ECR_MAX;

	// The surface hit object's type.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface")
	TEnumAsByte<EObjectTypeQuery> ObjectType = ObjectTypeQuery_MAX;

	// The surface type of the offset of this surface when traced.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface")
	ESurfaceTraceHitType SurfaceOffsetType = ESurfaceTraceHitType::NormalHit;

	// Is the component detecting the surface can character step on it?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface")
	bool bCanStepOn = false;

	// Get surface friction (X), surface bounciness (Y)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface")
	FVector SurfacePhysicProperties = FVector(1, 0, 0);


	// Update the tacking of the component.
	bool UpdateTracking(float deltaTime);

	// Update information about the hit
	void UpdateHit(FBodyInstance physicBody, FHitResult hit, ESurfaceTraceHitType offsetType);

	// Apply a force on the surface at a point on it and return the velocity of the surface at the point before force application. use reaction to apply force only if it's opposed to the surface normal
	FVector ApplyForceAtOnSurface(const FVector point, const FVector force, bool reactionForce = false) const;

	// Get the velocity planed on the surface normal. reaction planar return the same velocity if the dot product with normal > 0.
	FVector GetVelocityAlongNormal(const FVector velocity, const bool useImpactNormal = false, const bool reactionPlanarOnly = false) const;

	// Get the velocity at a point on the surface. in cm/sec
	FVector GetVelocityAt(const FVector point, const float deltaTime = 0) const;


	//Linear Velocity
public:
	//The linear velocity in Cm/s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Surface")
	FVector LinearVelocity = FVector(0);

private:
	FVector _lastPosition = FVector(NAN);

	//Angular Velocity
public:
	// The angular velocity (axis * angle) in Deg/s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Surface")
	FVector AngularVelocity = FVector(0);

private:
	FQuat _lastRotation = FQuat(NAN,NAN,NAN,NAN);
};


#pragma endregion

#pragma region Classes


// The Modular Mover movement mode asset class.
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover | Movement Modes", abstract)
class MODULARMOVER_API UBaseMoverMovementMode : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBaseMoverMovementMode();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FName ModeName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	int Priority = -1;

	FTraceDelegate OnQueryEnds;


public:
	UFUNCTION(BlueprintPure, Category="Modular Mover | Mover Movement Mode", meta = (CompactNodeTitle = "ValidState", BlueprintThreadSafe))
	bool IsValid() const;

	void TestTrace(UWorld* World, FString message, FTransform tr, FCollisionShape shape) const;


	void QueryResponse(const FTraceHandle& handle, FTraceDatum& datas);
};

#pragma endregion
