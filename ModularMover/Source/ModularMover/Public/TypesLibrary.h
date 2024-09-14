// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CollisionShape.h"
#include "CommonToolboxTypeLibrary.h"
#include "Engine/DataAsset.h"
#include "UObject/ObjectMacros.h"
#include "WorldCollision.h"
#include "TypesLibrary.generated.h"


#pragma region Enums

UENUM(BlueprintType)
enum class EDebugMode: uint8
{
	None,
	Spacial,
	SurfaceDetection,
	LinearMovement,
	AngularMovement,
};

#pragma endregion

#pragma region Structs

class UModularMoverComponent;
class UBaseMoverMovementMode;
class UBaseContingentMove;
class UBaseTransientMove;

USTRUCT(BlueprintType)
struct FTransientMovePhases
{
	GENERATED_BODY()

public:
	FORCEINLINE FTransientMovePhases()
	{
	}

	FORCEINLINE FTransientMovePhases(FName name, float duration)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FName PhaseName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float PhaseDuration = 0;
};

USTRUCT()
struct FMoveModeReference
{
	GENERATED_BODY()

public:
	FORCEINLINE FMoveModeReference()
	{
	}

	FORCEINLINE FMoveModeReference(UBaseMoverMovementMode* instance)
	{
		if (instance)
		{
			MovementModeInstance = instance;
			ReferenceCount = 1;
		}
	}

	UPROPERTY()
	TSoftObjectPtr<UBaseMoverMovementMode> MovementModeInstance = nullptr;

	UPROPERTY()
	int ReferenceCount = 0;
};

USTRUCT(BlueprintType)
struct FChunkAreaID
{
	GENERATED_BODY()

public:
	FORCEINLINE FChunkAreaID()
	{
	}

	FORCEINLINE FChunkAreaID(int x, int y, int z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoverChunk")
	int X = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoverChunk")
	int Y = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoverChunk")
	int Z = 0;

	FORCEINLINE bool Equals(const FChunkAreaID& other) const { return other.X == X && other.Y == Y && other.Z == Z; }

	FORCEINLINE bool operator==(const FChunkAreaID& other) const { return Equals(other); }

	FORCEINLINE bool operator!=(const FChunkAreaID& other) const { return !Equals(other); }
};


FORCEINLINE uint32 GetTypeHash(const FChunkAreaID& ChunkAreaID)
{
	uint32 Hash = FCrc::MemCrc32(&ChunkAreaID, sizeof(FChunkAreaID));
	return Hash;
}


USTRUCT(BlueprintType)
struct FMoverAreaChunk
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoverChunk")
	TArray<UModularMoverComponent*> Movers;
};


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


USTRUCT(BlueprintType)
struct FCommonMoveInfos
{
	GENERATED_BODY()

public:
	FCommonMoveInfos();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FName ModeName = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	int Priority = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FVector2D BlendTimes = FVector2D(0);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default", meta=(UIMin=0, UIMax=1, ClampMin=0, ClampMax=1))
	float CurrentWeight = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	float TimeOnMode = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	bool bIsActiveMode = false;
};


USTRUCT(BlueprintType)
struct FContingentMoveInfos
{
	GENERATED_BODY()

public:
	FContingentMoveInfos();

	FContingentMoveInfos(UBaseContingentMove* move);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FCommonMoveInfos BaseInfos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	float TotalTimeOnMode = 0;
};


USTRUCT(BlueprintType)
struct FTransientMoveInfos
{
	GENERATED_BODY()

public:
	FTransientMoveInfos();

	FTransientMoveInfos(UBaseTransientMove* move);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FCommonMoveInfos BaseInfos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	TArray<FTransientMovePhases> ModeDuration = TArray<FTransientMovePhases>{FTransientMovePhases("Default", 0.1)};
};


USTRUCT(BlueprintType)
struct FLinearMechanic
{
	GENERATED_BODY()

public:
	// The current acceleration to apply (cm/s-2)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector Acceleration = FVector(0);

	// Apply velocity instantaneously, ignoring acceleration. deceleration will also be instantaneous.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool InstantMode = false;

	// The terminal velocity (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float TerminalVelocity = 0;

	// The drag when force or Terminal Velocity are zero
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float StaticDrag = 1;
};

USTRUCT(BlueprintType)
struct FAngularMechanic
{
	GENERATED_BODY()

public:
	// The target look orientation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector LookDirection = FVector(0);

	// The actual rotation speed (deg/s). zero, negative values and excessive values will make the rotation instantaneous
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float TurnSpeed = 1;
};

USTRUCT(BlueprintType)
struct FMechanicProperties
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FLinearMechanic Linear = FLinearMechanic();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FAngularMechanic Angular = FAngularMechanic();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector Gravity = FVector(0, 0, -1);
};

USTRUCT(BlueprintType)
struct FMomentum
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FTransform Transform = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector LinearVelocity = FVector(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector AngularVelocity = FVector(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float Mass = 1;
};


#pragma endregion

#pragma region Classes


// The Modular Mover movement mode asset class.
UCLASS()
class UBaseMoverMovementMode : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBaseMoverMovementMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FName ModeName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	int Priority = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FVector2D BlendTimes = FVector2D(0);

	UFUNCTION(BlueprintPure, Category="Modular Mover | Mover Movement Mode", meta = (CompactNodeTitle = "ValidState", BlueprintThreadSafe))
	bool IsValid() const;
};


// The base of all contingent movement modes
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover | Movement Modes", abstract)
class MODULARMOVER_API UBaseContingentMove : public UBaseMoverMovementMode
{
	GENERATED_BODY()

public:
	UBaseContingentMove();
};


// The base of all transient movement modes
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover | Movement Modes", abstract)
class MODULARMOVER_API UBaseTransientMove : public UBaseMoverMovementMode
{
	GENERATED_BODY()

public:
	UBaseTransientMove();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timing")
	TArray<FTransientMovePhases> ModeDuration = TArray<FTransientMovePhases>{FTransientMovePhases("Default", 0.1)};
};

#pragma endregion
