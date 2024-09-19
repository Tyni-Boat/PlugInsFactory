// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "CommonToolboxTypeLibrary.h"
#include "ModularMoverSubsystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsPublic.h"
#include "TypesLibrary.h"
#include "Components/ActorComponent.h"
#include "GameFramework/MovementComponent.h"
#include "ModularMoverComponent.generated.h"


#define OVERLAP_INFLATION 10
#define DRAG_To_DAMPING 2.4026


class UModularMoverComponent;


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMoveDelegate, UModularMoverComponent*, FMoverCheckRequest)


UCLASS(ClassGroup = "Controllers",
	hidecategories = (Sockets, Object, LOD, Lighting, TextureStreaming, Velocity
		, PlanarMovement, ComponentTick, ComponentReplication, MovementComponent
		, Tags, Replication, Navigation, Activation, Cooking, AssetUserData, Collision),
	meta = (DisplayName = "Modular Controller", BlueprintSpawnableComponent))
class MODULARMOVER_API UModularMoverComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UModularMoverComponent();

	~UModularMoverComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Register the component to it's subsystem.
	bool InitialRegistration();

	// Unregister the component to it's subsystem.
	bool FinalUnregistration();

	// The reference to the current subsystem.
	UPROPERTY()
	TSoftObjectPtr<UModularMoverSubsystem> _subSystem = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Debug")
	EDebugMode DebugMode = EDebugMode::None;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void AsyncPhysicsTickComponent(float DeltaTime, float SimTime) override;

	// Evaluate continuous and instantaneous movement modes as well as Traversals
	void EvaluateMovementOnSurfaces(const FMoverCheckRequest Request, TArray<FExpandedHitResult> SurfacesHits);


#pragma region Inputs
public:
	// Set world space Move input to this Mover component
	UFUNCTION(BlueprintCallable, Category="Inputs|Writing")
	void MoveComponent(const FVector Movement);

	// Add a Vector input. Add a life time to make it persist for the set duration (sec)
	UFUNCTION(BlueprintCallable, Category="Inputs|Writing")
	void AddVectorInput(const FName InputName, const FVector Vector, const float LifeTime = 0);

	// Add a Rotation input. Add a life time to make it persist for the set duration (sec)
	UFUNCTION(BlueprintCallable, Category="Inputs|Writing")
	void AddRotationInput(const FName InputName, const FRotator Rotation, const float LifeTime = 0);

	// Add a Value input. Add a life time to make it persist for the set duration (sec)
	UFUNCTION(BlueprintCallable, Category="Inputs|Writing")
	void AddValueInput(const FName InputName, const float Value, const float LifeTime = 0);

	// Add a Trigger input. Add a life time to make it persist for the set duration (sec)
	UFUNCTION(BlueprintCallable, Category="Inputs|Writing")
	void AddTriggerInput(const FName InputName, const float LifeTime = 0);


	// Read a Vector input if it exist.
	UFUNCTION(BlueprintPure, Category="Inputs|Reading", meta=(BlueprintThreadSafe))
	bool ReadVectorInput(const FName InputName, UPARAM(ref) FVector& OutVector) const;

	// Read a Rotation input if it exist. 
	UFUNCTION(BlueprintPure, Category="Inputs|Reading", meta=(BlueprintThreadSafe))
	bool ReadRotationInput(const FName InputName, UPARAM(ref) FRotator& OutRotation) const;

	// Read a Value input if it exist.
	UFUNCTION(BlueprintPure, Category="Inputs|Reading", meta=(BlueprintThreadSafe))
	bool ReadValueInput(const FName InputName, UPARAM(ref) float& OutValue) const;

	// Read a Trigger input if it exist.
	UFUNCTION(BlueprintPure, Category="Inputs|Reading", meta=(BlueprintThreadSafe))
	bool ReadTriggerInput(const FName InputName, UPARAM(ref) bool& OutTrigger) const;

protected:
	UPROPERTY()
	FVector _movementInput;
	UPROPERTY()
	FMoverInputPool _inputPool;
	UPROPERTY()
	TArray<FName> _inputClearList;

	// Update inputs
	void UpdateInputs(float deltaTime);

#pragma endregion


#pragma region Physic

public:
	// Called when ever the location of the controller changed.
	FOnMoveDelegate OnComponentMoved;

	// The rotation offset. helpful when using rotated skeletal mesh components 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Physic|Spacial")
	FRotator RotationOffset;

	// The current area chunk
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Physic|Spacial")
	FChunkAreaID CurrentAreaChunk;

	// The distance tolerance to trigger the component moved event
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Physic|Spacial")
	float MoveTriggerTolerance = 0.1;

	// The angular tolerance to trigger the component moved event. (degrees)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Physic|Spacial")
	float RotateTriggerTolerance = 15;

	// Disable collision with the world and other characters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	bool bDisableCollision = false;

	// Use asynchronous surface detection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	bool bUseAsyncSurfaceDetection = false;

	// The list of components to ignore when moving.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	TArray<UPrimitiveComponent*> IgnoredCollisionComponents;

	// Use complex collision for movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	bool bUseComplexCollision = false;

	// The minimum depth when detecting depth surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	float MinDepthSurface = 10;

	// The component mass
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|RigidBody")
	float Mass = 80;


protected:
	UPROPERTY()
	UPhysicalMaterial* _customPhysicMaterial = nullptr;
	FVector _lastLocation;
	FVector _lastLocation_trigger;
	FQuat _lastRotation;
	FQuat _lastRotation_trigger;
	FTraceDelegate _onMainSurfaceChk;
	TMap<FTraceHandle, FMoverCheckRequest> _chkRequestMap;
	FVector _lastGravity = FVector(0, 0, -1);
	bool _bMoveDisableCollision = false;
	UPROPERTY()
	TMap<UPrimitiveComponent*, FTransform> _lastSurfaceTransform;


	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	bool IsIgnoringCollision() const;

	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	FVector GetCurrentScanSurfaceVector() const;

	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	float GetCurrentScanSurfaceOffset() const;

	// Get the controller Mass
	UFUNCTION(BlueprintPure, Category = "Mover|Physic")
	FORCEINLINE float GetMass() const
	{
		return (Mass < 0 && UpdatedPrimitive != nullptr && UpdatedPrimitive->IsSimulatingPhysics()) ? UpdatedPrimitive->GetMass() : FMath::Clamp(Mass, 1, TNumericLimits<double>().Max());
	}

	// Check movement diff and trigger on move events
	void EvaluateMovementDiff(FBodyInstance* Body, const FMomentum Momentum, const FMoverInputPool InputPool);

	// Check if a surface transform changed enough to trigger movement eval.
	bool TrackSurfaceMovementUpdate(const TArray<FSurface>& Surfaces);

	// Run the solver when ever moved
	void OnMoveCheck(UModularMoverComponent* Mover, FMoverCheckRequest Request);

	// Called when we've done the surface check trace async
	void OnMainSurfaceCheckDone(const FTraceHandle& TraceHandle, FTraceDatum& TraceData);

	//Solve overlap problems recursively.
	void OverlapSolver(int& maxDepth, const FTransform* customTr = nullptr) const;

	// Detect surface overlap hits
	bool DetectOverlapHits(const FTransform Transform, TArray<FExpandedHitResult>& touchedHits, const FVector scanVector = FVector(0), const uint64 CounterDirectionOffset = 0) const;

	// Detect surface overlap hits async
	FTraceHandle AsyncDetectOverlapHits(const FTransform Transform, FTraceDelegate* callBack, const FVector scanVector = FVector(0), const uint64 CounterDirectionOffset = 0) const;

	// De-penetrate overlaps
	void FixOverlapHits(int& maxDepth, const FTransform Transform, const TArray<FExpandedHitResult> touchedHits, std::function<void(FVector)> OnLocationSet = {},
	                    std::function<void(UPrimitiveComponent*, FVector)> OnPhysicCompHit = {}) const;

	// Calculate Linear velocity. UseReduction subtract the current Linear Velocity from the end result.
	static FVector ComputeLinearVelocity(FLinearMechanic AttemptedMovement, FVector currentLinearVelocity, const float deltaTime, bool UseReduction = false);

	// Calculate Angular velocity (Rad/s). UseReduction subtract the current Angular Velocity from the end result.
	static FVector ComputeAngularVelocity(FAngularMechanic AttemptedMovement, FVector CurrentAngularVelocity, const FQuat CurrentOrientation, FVector Gravity, const float deltaTime,
	                                      bool UseReduction = false, const FRotator OffsetRotation = FRotator(0));

#pragma endregion

#pragma region Movement Modes


public:
	// Custom Properties. help when checking move validity. can represent anything, from bool, values, vector, rotation, etc...
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, category = "Movement Modes")
	TMap<FName, FVector> CustomProperties;

	// The currently active contingent move mode.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, category = "Movement Modes|Contingent")
	FMoverModeSelection ActiveContingentMove;

	// The currently active Transient move mode.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, category = "Movement Modes|Transient")
	FMoverModeSelection ActiveTransientMove;

protected:
	TQueue<FVector> _chkContingentQueue;
	TQueue<FVector> _chkTransientQueue;


	// Contingents ------------------------------------------------------------------------------------------------------------------------------------------
public:
	// The contingent move modes used on this controller by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Movement Modes|Contingent")
	TArray<TSubclassOf<UBaseContingentMove>> ContingentMoveClasses;

	// The states of each contingent move for this mover
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, category = "Movement Modes|Contingent")
	TArray<FContingentMoveInfos> ContingentMoveState;


	// Add a contingent move mode
	UFUNCTION(BlueprintCallable, Category="Mover|Movement Modes|Contingent", meta=(NotBlueprintThreadSafe))
	void AddContingentMoveMode(TSubclassOf<UBaseContingentMove> Class);

	// Remove a contingent move mode
	UFUNCTION(BlueprintCallable, Category="Mover|Movement Modes|Contingent", meta=(NotBlueprintThreadSafe))
	void RemoveContingentMoveMode(FName MoveName);


	// Check contingent moves and select active contingent mode.
	void CheckContingentMoves(const FMoverCheckRequest Request, const TArray<FExpandedHitResult> SurfacesHits);


	// Process and blend actives contingent movements in an async manner (transitions from mode to mode are done at different speeds)
	FMechanicProperties ProcessContingentMoves(const FMomentum currentMomentum, const float DeltaTime);

	// Transients ------------------------------------------------------------------------------------------------------------------------------------------

public:
	// The Transient move modes used on this controller by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Movement Modes|Transient")
	TArray<TSubclassOf<UBaseTransientMove>> TransientMoveClasses;

	// The states of each Transient move for this mover
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, category = "Movement Modes|Transient")
	TArray<FTransientMoveInfos> TransientMoveState;


	// Add a Transient move mode
	UFUNCTION(BlueprintCallable, Category="Mover|Movement Modes|Contingent", meta=(NotBlueprintThreadSafe))
	void AddTransientMoveMode(TSubclassOf<UBaseTransientMove> Class);

	// Remove a Transient move mode
	UFUNCTION(BlueprintCallable, Category="Mover|Movement Modes|Contingent", meta=(NotBlueprintThreadSafe))
	void RemoveTransientMoveMode(FName MoveName);


	// Check Transient moves and select active Transient mode.
	void CheckTransientMoves(const FMoverCheckRequest Request, const TArray<FExpandedHitResult> Surfaces);

	// Process and blend actives Transient movements in a sync manner (transitions from mode to mode are done at the same speed)
	FMechanicProperties ProcessTransientMoves(const FMechanicProperties ContingentMoveResult, const FMomentum currentMomentum, const float DeltaTime);

	// Traversal ------------------------------------------------------------------------------------------------------------------------------------------

#pragma endregion

#pragma region Flow Mode

	// Stand alone mode

#pragma endregion

#pragma region Movement

	// Move a body according to Movement
	void MoveBody(FBodyInstance* Body, const FMechanicProperties movement, const float Delta) const;

	// Get the orientation, keeping body upright.
	static bool GetAngularOrientation(FQuat& Orientation, const FQuat BodyOrientation, const FAngularMechanic angularMechanic, const FVector Gravity,
	                                  const FRotator OffsetRotation = FRotator(0));

	// Calculate an Angular Velocity (Rad/s) to fit orientation, keeping body upright.
	static FVector GetAngularVelocity(const FQuat BodyOrientation, const FAngularMechanic angularMechanic, const FVector Gravity, const FRotator OffsetRotation = FRotator(0));

	// Get the trajectory
	UFUNCTION(BlueprintPure, Category="Mover|Movement")
	static TArray<FMomentum> GetTrajectory(const int SampleCount, const FMechanicProperties inputMovement, const FMomentum currentMomentum, const float timeStep,
	                                       const FRotator OffsetRotation);

#pragma endregion
};
