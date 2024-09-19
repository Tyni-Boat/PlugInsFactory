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
	MovementProcessing,
	Inputs,
};

UENUM(BlueprintType)
enum class EInputType: uint8
{
	None,
	Vector,
	Value,
	Trigger,
	Rotation,
};

UENUM(BlueprintType)
enum class ECollisionShapeNature: uint8
{
	Line,
	Box,
	Sphere,
	Capsule
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
	FVector2D BlendSpeed = FVector2D(0);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FTransientMovePhases CurrentPhase = FTransientMovePhases();

	FORCEINLINE float TotalDuration() const
	{
		float duration = 0;
		if (ModeDuration.Num() <= 0)
			return duration;
		for (auto dur : ModeDuration)
			duration += dur.PhaseDuration;
		return duration;
	}

	FORCEINLINE void UpdatePhase()
	{
		float collectiveTime = 0;
		CurrentPhase = FTransientMovePhases();

		for (int i = 0; i < ModeDuration.Num(); i++)
		{
			collectiveTime += ModeDuration[i].PhaseDuration;
			if (BaseInfos.TimeOnMode <= collectiveTime)
			{
				CurrentPhase = ModeDuration[i];
				break;
			}
		}
	}
};


USTRUCT(BlueprintType)
struct FLinearMechanic
{
	GENERATED_BODY()

public:
	// The current acceleration to apply (cm/s-2)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector Acceleration = FVector(0);
	
	// The Snap vector. instantaneously displace the body teleporting physic. can cause clipping, use with care 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector SnapDisplacement = FVector(0);

	// The terminal velocity (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float TerminalVelocity = 0;

	// The Deceleration speed (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float DecelerationSpeed = 100;
};

USTRUCT(BlueprintType)
struct FAngularMechanic
{
	GENERATED_BODY()

public:
	// The Look rotation, used to calculate Orientation Diff whenever it is Identity Quaternion. The lenght of the vector will be the rotation speed (Deg/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector LookOrientation = FVector(0);

	// The target orientation difference to apply.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FQuat OrientationDiff = FQuat::Identity;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool IgnoreCollision = false;

	FORCEINLINE void Scale(const float amount)
	{
		Linear.Acceleration *= amount;
		Linear.DecelerationSpeed *= amount;
		Linear.TerminalVelocity *= amount;
		Angular.LookOrientation *= amount;
		FVector axis;
		float angle;
		Angular.OrientationDiff.ToAxisAndAngle(axis, angle);
		Angular.OrientationDiff = FQuat(axis, angle * amount);
		Gravity *= amount;
	}

	FORCEINLINE FMechanicProperties operator+(const FMechanicProperties& other)
	{
		FMechanicProperties result;
		result.Linear.Acceleration = this->Linear.Acceleration + other.Linear.Acceleration;
		result.Linear.DecelerationSpeed = this->Linear.DecelerationSpeed + other.Linear.DecelerationSpeed;
		result.Linear.TerminalVelocity = this->Linear.TerminalVelocity + other.Linear.TerminalVelocity;

		result.Angular.LookOrientation = this->Angular.LookOrientation + other.Angular.LookOrientation;
		result.Angular.OrientationDiff = this->Angular.OrientationDiff * other.Angular.OrientationDiff;

		result.Gravity = this->Gravity + other.Gravity;
		result.IgnoreCollision = this->IgnoreCollision | other.IgnoreCollision;
		return result;
	}
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector Gravity = FVector(0, 0, -1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	TArray<FExpandedHitResult> Surfaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	ECollisionShapeNature ShapeType = ECollisionShapeNature::Line;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector ShapeExtends = FVector(0);


	FORCEINLINE FCollisionShape GetShape() const
	{
		switch (ShapeType)
		{
			case ECollisionShapeNature::Line: break;
			case ECollisionShapeNature::Box: return FCollisionShape::MakeBox(ShapeExtends);
			case ECollisionShapeNature::Sphere: return FCollisionShape::MakeSphere(ShapeExtends.X);
			case ECollisionShapeNature::Capsule: return FCollisionShape::MakeCapsule(ShapeExtends);
		}
		return FCollisionShape();
	}

	FORCEINLINE void SetShape(FCollisionShape Shape)
	{
		ShapeExtends = Shape.GetExtent();
		switch (Shape.ShapeType)
		{
			case ECollisionShape::Line: ShapeType = ECollisionShapeNature::Line;
				break;
			case ECollisionShape::Box: ShapeType = ECollisionShapeNature::Box;
				break;
			case ECollisionShape::Sphere:
				{
					ShapeType = ECollisionShapeNature::Sphere;
					ShapeExtends.X = Shape.GetSphereRadius();
				}
				break;
			case ECollisionShape::Capsule: ShapeType = ECollisionShapeNature::Capsule;
				break;
		}
	}
};

USTRUCT(BlueprintType)
struct FMoverInput
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector Property = FVector(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	EInputType Type = EInputType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float LifeTime = 0;
};

USTRUCT(BlueprintType)
struct FMoverInputPool
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	TMap<FName, FMoverInput> InputMap;
};

USTRUCT(BlueprintType)
struct FMoverModeSelection
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FName ModeName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	TArray<FExpandedHitResult> Surfaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector ScanSurfaceVector = FVector(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float ScanSurfaceOffset = 0;

	// Enabled when the mover already began processing this move.
	bool bConsumedActivation = false;


	FORCEINLINE bool IsValid() const { return ModeName.IsValid() && !ModeName.IsNone(); };
};

USTRUCT(BlueprintType)
struct FMoverCheckRequest
{
	GENERATED_BODY()

public:
	FORCEINLINE FMoverCheckRequest()
	{
	}

	FORCEINLINE FMoverCheckRequest(const FMomentum& momentum, FMoverInputPool inputs)
	{
		Momentum = momentum;
		InputPool.InputMap.Empty();
		for (auto input : inputs.InputMap)
			InputPool.InputMap.Add(input);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FMomentum Momentum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FMoverInputPool InputPool;
};


// Expansion Functions for structures
UCLASS()
class MODULARMOVER_API UStructExtension : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "StructExtension|Inputs", meta=(BlueprintThreadSafe))
	static bool ReadVectorInput(const FMoverInputPool InputPool, const FName InputName, FVector& OutVector);

	UFUNCTION(BlueprintPure, Category = "StructExtension|Inputs", meta=(BlueprintThreadSafe))
	static bool ReadRotationInput(const FMoverInputPool InputPool, const FName InputName, FRotator& OutRotation);

	UFUNCTION(BlueprintPure, Category = "StructExtension|Inputs", meta=(BlueprintThreadSafe))
	static bool ReadValueInput(const FMoverInputPool InputPool, const FName InputName, float& OutValue);

	UFUNCTION(BlueprintPure, Category = "StructExtension|Inputs", meta=(BlueprintThreadSafe))
	static bool ReadTriggerInput(const FMoverInputPool InputPool, const FName InputName, bool& OutTrigger);
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
	FVector2D BlendSpeed = FVector2D(10);

	UFUNCTION(BlueprintPure, Category="Modular Mover | Mover Movement Mode", meta = (CompactNodeTitle = "ValidMoveMode", BlueprintThreadSafe))
	bool IsValid() const;
};


// The base of all contingent movement modes
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover | Movement Modes", abstract)
class MODULARMOVER_API UBaseContingentMove : public UBaseMoverMovementMode
{
	GENERATED_BODY()

public:
	UBaseContingentMove();

	// The direction where to scan for surfaces
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FVector ScanSurfaceVector = FVector(0);

	// The offset in the counter direction where to scan for surfaces
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	float ScanSurfaceOffset = 0;


	UFUNCTION(BlueprintNativeEvent, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe, AdvancedDisplay = 5))
	bool CheckContingentMovement(UPARAM(ref) const TArray<FExpandedHitResult>& Surfaces, UPARAM(ref) FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                             const FVector MoveInput,
	                             const FMoverInputPool Inputs, UPARAM(ref) const TArray<FContingentMoveInfos>& ContingentMoves,
	                             UPARAM(ref) const TArray<FTransientMoveInfos>& TransientMoves, UPARAM(ref) TMap<FName, FVector>& CustomProperties, UPARAM(ref) int& SurfacesFlag) const;

	UFUNCTION(BlueprintCallable, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe, AdvancedDisplay = 5))
	virtual bool CheckContingentMovement_Implementation(UPARAM(ref) const TArray<FExpandedHitResult>& Surfaces, UPARAM(ref) FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                                    const FVector MoveInput,
	                                                    const FMoverInputPool Inputs,UPARAM(ref) const TArray<FContingentMoveInfos>& ContingentMoves,
	                                                    UPARAM(ref) const TArray<FTransientMoveInfos>& TransientMoves, UPARAM(ref) TMap<FName, FVector>& CustomProperties,
	                                                    UPARAM(ref) int& SurfacesFlag) const;


	UFUNCTION(BlueprintNativeEvent, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe))
	FMechanicProperties ProcessContingentMovement(UPARAM(ref) FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput, const FMoverInputPool Inputs,
	                                              const float DeltaTime) const;

	UFUNCTION(BlueprintCallable, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe))
	virtual FMechanicProperties ProcessContingentMovement_Implementation(UPARAM(ref) FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
	                                                                     const FMoverInputPool Inputs, const float DeltaTime) const;
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


	UFUNCTION(BlueprintNativeEvent, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe, AdvancedDisplay = 5))
	bool CheckTransientMovement(UPARAM(ref) const TArray<FExpandedHitResult>& Surfaces, UPARAM(ref) FTransientMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
	                            const FMoverInputPool Inputs, UPARAM(ref) const TArray<FContingentMoveInfos>& ContingentMoves,
	                            UPARAM(ref) const TArray<FTransientMoveInfos>& TransientMoves, UPARAM(ref) TMap<FName, FVector>& CustomProperties, UPARAM(ref) int& SurfacesFlag) const;

	UFUNCTION(BlueprintCallable, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe, AdvancedDisplay = 5))
	virtual bool CheckTransientMovement_Implementation(UPARAM(ref) const TArray<FExpandedHitResult>& Surfaces, UPARAM(ref) FTransientMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                                   const FVector MoveInput,
	                                                   const FMoverInputPool Inputs, UPARAM(ref) const TArray<FContingentMoveInfos>& ContingentMoves,
	                                                   UPARAM(ref) const TArray<FTransientMoveInfos>& TransientMoves, UPARAM(ref) TMap<FName, FVector>& CustomProperties,
	                                                   UPARAM(ref) int& SurfacesFlag) const;


	UFUNCTION(BlueprintNativeEvent, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe))
	FMechanicProperties ProcessTransientMovement(const FMechanicProperties ContingentMove, UPARAM(ref) FTransientMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                             const FVector MoveInput,
	                                             const FMoverInputPool Inputs,
	                                             const float DeltaTime) const;

	UFUNCTION(BlueprintCallable, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe))
	virtual FMechanicProperties ProcessTransientMovement_Implementation(const FMechanicProperties ContingentMove, UPARAM(ref) FTransientMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                                                    const FVector MoveInput,
	                                                                    const FMoverInputPool Inputs, const float DeltaTime) const;
};

#pragma endregion
