// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include <functional>

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
class UBaseContinuousMove;
class UBaseTemporaryMove;

USTRUCT(BlueprintType)
struct FTemporaryMovePhases
{
	GENERATED_BODY()

public:
	FORCEINLINE FTemporaryMovePhases()
	{
	}

	FORCEINLINE FTemporaryMovePhases(FName name, float duration)
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
struct FMoverHitResult
{
	GENERATED_BODY()

	FMoverHitResult();
	
	FMoverHitResult(FHitResult hit, FName owner, FCollisionShape shape);
	
	//The HitResult
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	FHitResult Hit = FHitResult();
	
	//The name of the movement "Owning" this Hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	FName OwningMovement = "";

	FCollisionShape Shape;
};

USTRUCT(BlueprintType)
struct FDebugItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FLinearColor Color = FColor::White;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float Radius = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float Thickness = 1;
	
};

// Linear and angular Velocity of a surface.
USTRUCT(BlueprintType)
struct FSurfaceVelocity
{
	GENERATED_BODY()

public:

	FSurfaceVelocity();
	
	FSurfaceVelocity(const FHitResult& hit);
	
	// Update the tacking of the component.
	bool UpdateTracking(float deltaTime);

	// Get the velocity at a point on the surface. in cm/sec
	FVector GetVelocityAt(const FVector point, const FVector Normal = FVector(0), const float deltaTime = 0) const;
	
	
	//The linear velocity in Cm/s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Surface")
	TWeakObjectPtr<UPrimitiveComponent> Component = nullptr;
	
	//The linear velocity in Cm/s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Surface")
	FVector LinearVelocity = FVector(0);

	// The angular velocity (axis * angle) in Deg/s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Surface")
	FVector AngularVelocity = FVector(0);

	// The target bone.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Surface")
	FName BoneName = NAME_None;

private:
	FVector _lastPosition = FVector(NAN);
	FQuat _lastRotation = FQuat(NAN,NAN,NAN,NAN);
};


USTRUCT(BlueprintType)
struct MODULARMOVER_API FSurface
{
	GENERATED_BODY()

	FSurface();

	FSurface(const FBodyInstance* physicBody, FHitResult hit, ESurfaceTraceHitType offsetType = ESurfaceTraceHitType::NormalHit, float Depth = 0, std::function<void()> OnBoneChanged = nullptr);

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

	// The hit depth
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface")
	float HitDepth = 0;

	// Get surface friction (X), surface bounciness (Y)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface")
	FVector SurfacePhysicProperties = FVector(1, 0, 0);



	// Update information about the hit
	void UpdateHit(const FBodyInstance* physicBody, FHitResult hit, ESurfaceTraceHitType offsetType, std::function<void()> OnBoneChanged = nullptr);

	// Apply a force on the surface at a point on it and return the velocity of the surface at the point before force application. use reaction to apply force only if it's opposed to the surface normal
	FVector ApplyForceAtOnSurface(const FVector point, const FVector force, bool reactionForce = false) const;

	// Get the velocity planed on the surface normal. reaction planar return the same velocity if the dot product with normal > 0.
	FVector GetVelocityAlongNormal(const FVector velocity, const bool useImpactNormal = false, const bool reactionPlanarOnly = false) const;
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
struct FContinuousMoveInfos
{
	GENERATED_BODY()

public:
	FContinuousMoveInfos();

	FContinuousMoveInfos(UBaseContinuousMove* move);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FCommonMoveInfos BaseInfos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	float TotalTimeOnMode = 0;
};


USTRUCT(BlueprintType)
struct FTemporaryMoveInfos
{
	GENERATED_BODY()

public:
	FTemporaryMoveInfos();

	FTemporaryMoveInfos(UBaseTemporaryMove* move);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FCommonMoveInfos BaseInfos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	TArray<FTemporaryMovePhases> ModeDuration = TArray<FTemporaryMovePhases>{FTemporaryMovePhases("Default", 0.1)};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FTemporaryMovePhases CurrentPhase = FTemporaryMovePhases();

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
		CurrentPhase = FTemporaryMovePhases();

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
	FTransform SurfacesMovement = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector Gravity = FVector(0, 0, -1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool IgnoreCollision = false;

	FORCEINLINE void Scale(const float amount)
	{
		Linear.Acceleration *= amount;
		Linear.SnapDisplacement *= amount;
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
		result.Linear.SnapDisplacement = this->Linear.SnapDisplacement + other.Linear.SnapDisplacement;

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
	FSurface Surface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector ScanSurfaceVector = FVector(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float ScanSurfaceOffset = 0;

	// Enabled when the mover already began processing this move.
	bool bConsumedActivation = false;


	FORCEINLINE bool IsValid() const { return ModeName.IsValid() && !ModeName.IsNone(); };
};

USTRUCT(BlueprintType)
struct FMoverRequest
{
	GENERATED_BODY()

public:
	FORCEINLINE FMoverRequest()
	{
	}

	FORCEINLINE FMoverRequest(FBodyInstance* moverBody, const FMomentum& momentum, FMoverInputPool inputs, float delta, FVector scanDir)
	{
		MoverBody = moverBody;
		Momentum = momentum;
		ScanDirection = scanDir;
		DeltaTime = delta;
		InputPool.InputMap.Empty();
		for (auto input : inputs.InputMap)
			InputPool.InputMap.Add(input);
	}

	FBodyInstance* MoverBody = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FMomentum Momentum = FMomentum();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FMoverInputPool InputPool = FMoverInputPool();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FVector ScanDirection = FVector(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float DeltaTime = 0;
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


	//Apply force on surfaces at a point.
	UFUNCTION(BlueprintCallable, Category = "StructExtension|Inputs")
	static void ApplyForceOnSurfaces(TArray<FSurface>& Surfaces, const FVector point, const FVector force, bool reactionForce, ECollisionResponse channelFilter);

	//Get surface velocity along hit normal
	UFUNCTION(BlueprintPure, Category = "StructExtension|Inputs", meta=(BlueprintThreadSafe))
	static FVector GetSurfacesVelocityFromNormal(const TArray<FSurface>& Surfaces, const FVector velocity, const bool useImpactNormal, ECollisionResponse channelFilter);

	//Get surface combined surface average linear velocity
	UFUNCTION(BlueprintPure, Category = "StructExtension|Inputs", meta=(BlueprintThreadSafe))
	static FVector GetAverageSurfaceVelocityAt(const TArray<FSurfaceVelocity>& SurfacesMobilities, const FVector point, const float deltaTime, FVector Normal = FVector(0));

	//Get surface combined surface average angular velocity. (Deg/s)
	UFUNCTION(BlueprintPure, Category = "StructExtension|Inputs", meta=(BlueprintThreadSafe))
	static FVector GetAverageSurfaceAngularSpeed(const TArray<FSurfaceVelocity>& SurfacesMobilities);

	//Get surface highest physic properties (X) friction, (Y) bounciness.
	UFUNCTION(BlueprintPure, Category = "StructExtension|Inputs", meta=(BlueprintThreadSafe))
	static FVector GetMaxSurfacePhysicProperties(const TArray<FSurface>& Surfaces, ECollisionResponse channelFilter);
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


// The base of all Continuous movement modes
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover | Movement Modes", abstract)
class MODULARMOVER_API UBaseContinuousMove : public UBaseMoverMovementMode
{
	GENERATED_BODY()

public:
	UBaseContinuousMove();

	// The direction where to scan for surfaces
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FVector ScanSurfaceVector = FVector(0);

	// The offset in the counter direction where to scan for surfaces. Can Add overheat, don't use if possible.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	float ScanSurfaceOffset = 0;


	UFUNCTION(BlueprintNativeEvent, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe, AdvancedDisplay = 5))
	bool CheckContinuousMovement(UActorComponent* MoverActorComponent, UPARAM(ref) const TArray<FMoverHitResult>& Surfaces, UPARAM(ref) FContinuousMoveInfos& MoveInfos,
	                             const FMomentum& CurrentMomentum,
	                             const FVector MoveInput,
	                             const FMoverInputPool Inputs, UPARAM(ref) const TArray<FContinuousMoveInfos>& ContinuousMoves,
	                             UPARAM(ref) const TArray<FTemporaryMoveInfos>& TemporaryMoves, UPARAM(ref) TMap<FName, FVector>& CustomProperties, UPARAM(ref) int& SurfacesFlag) const;

	UFUNCTION(BlueprintCallable, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe, AdvancedDisplay = 5))
	virtual bool CheckContinuousMovement_Implementation(UActorComponent* MoverActorComponent,UPARAM(ref) const TArray<FMoverHitResult>& Surfaces,
	                                                    UPARAM(ref) FContinuousMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                                    const FVector MoveInput,
	                                                    const FMoverInputPool Inputs,UPARAM(ref) const TArray<FContinuousMoveInfos>& ContinuousMoves,
	                                                    UPARAM(ref) const TArray<FTemporaryMoveInfos>& TemporaryMoves, UPARAM(ref) TMap<FName, FVector>& CustomProperties,
	                                                    UPARAM(ref) int& SurfacesFlag) const;


	UFUNCTION(BlueprintNativeEvent, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe))
	FMechanicProperties ProcessContinuousMovement(UActorComponent* MoverActorComponent,UPARAM(ref) FContinuousMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                              const FVector MoveInput, const FMoverInputPool Inputs, const FTransform SurfacesMovement,
	                                              const float DeltaTime) const;

	UFUNCTION(BlueprintCallable, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe))
	virtual FMechanicProperties ProcessContinuousMovement_Implementation(UActorComponent* MoverActorComponent,UPARAM(ref) FContinuousMoveInfos& MoveInfos,
	                                                                     const FMomentum& CurrentMomentum, const FVector MoveInput,
	                                                                     const FMoverInputPool Inputs, const FTransform SurfacesMovement, const float DeltaTime) const;
};


// The base of all Temporary movement modes
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover | Movement Modes", abstract)
class MODULARMOVER_API UBaseTemporaryMove : public UBaseMoverMovementMode
{
	GENERATED_BODY()

public:
	UBaseTemporaryMove();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timing")
	TArray<FTemporaryMovePhases> ModeDuration = TArray<FTemporaryMovePhases>{FTemporaryMovePhases("Default", 0.1)};


	UFUNCTION(BlueprintNativeEvent, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe, AdvancedDisplay = 5))
	bool CheckTemporaryMovement(UActorComponent* MoverActorComponent,UPARAM(ref) const TArray<FExpandedHitResult>& Surfaces, UPARAM(ref) FTemporaryMoveInfos& MoveInfos,
	                            const FMomentum& CurrentMomentum, const FVector MoveInput,
	                            const FMoverInputPool Inputs, UPARAM(ref) const TArray<FContinuousMoveInfos>& ContinuousMoves,
	                            UPARAM(ref) const TArray<FTemporaryMoveInfos>& TemporaryMoves, UPARAM(ref) TMap<FName, FVector>& CustomProperties) const;

	UFUNCTION(BlueprintCallable, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe, AdvancedDisplay = 5))
	virtual bool CheckTemporaryMovement_Implementation(UActorComponent* MoverActorComponent,UPARAM(ref) const TArray<FExpandedHitResult>& Surfaces,
	                                                   UPARAM(ref) FTemporaryMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                                   const FVector MoveInput,
	                                                   const FMoverInputPool Inputs, UPARAM(ref) const TArray<FContinuousMoveInfos>& ContinuousMoves,
	                                                   UPARAM(ref) const TArray<FTemporaryMoveInfos>& TemporaryMoves, UPARAM(ref) TMap<FName, FVector>& CustomProperties) const;


	UFUNCTION(BlueprintNativeEvent, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe))
	FMechanicProperties ProcessTemporaryMovement(UActorComponent* MoverActorComponent, const FMechanicProperties ContinuousMove, UPARAM(ref) FTemporaryMoveInfos& MoveInfos,
	                                             const FMomentum& CurrentMomentum,
	                                             const FVector MoveInput,
	                                             const FMoverInputPool Inputs, const FTransform SurfacesMovement,
	                                             const float DeltaTime) const;

	UFUNCTION(BlueprintCallable, Category="Modular Mover | Mover Movement Mode", meta = (BlueprintThreadSafe))
	virtual FMechanicProperties ProcessTemporaryMovement_Implementation(UActorComponent* MoverActorComponent, const FMechanicProperties ContinuousMove,
	                                                                    UPARAM(ref) FTemporaryMoveInfos& MoveInfos,
	                                                                    const FMomentum& CurrentMomentum,
	                                                                    const FVector MoveInput,
	                                                                    const FMoverInputPool Inputs, const FTransform SurfacesMovement, const float DeltaTime) const;
};

#pragma endregion
