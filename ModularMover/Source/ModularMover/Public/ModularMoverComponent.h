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
#include "GameFramework/NavMovementComponent.h"
#include "ModularMoverComponent.generated.h"


class UModularMoverComponent;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FOnSurfaceCheckSignature, UModularMoverComponent, OnSurfaceCheck, FMomentum, Momentum, TArray<FMoverHitResult>&, HitResults);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FOnContinuousChangedSignature, UModularMoverComponent, OnContinuousMoveChanged, FMoverModeSelection, Old, FMoverModeSelection, New);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FOnTemporaryChangedSignature, UModularMoverComponent, OnTemporaryMoveChanged, FMoverModeSelection, Old, FMoverModeSelection, New);


#define COMPONENT_MAIN_INFLATION 1


UCLASS(ClassGroup = "Controllers",
	hidecategories = (Sockets, Object, LOD, Lighting, TextureStreaming, Velocity
		, PlanarMovement, ComponentTick, ComponentReplication, MovementComponent
		, Tags, Replication, Navigation, Activation, Cooking, AssetUserData, Collision, NavMovement),
	meta = (DisplayName = "Modular Controller", BlueprintSpawnableComponent))
class MODULARMOVER_API UModularMoverComponent : public UNavMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UModularMoverComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Initialize component.
	bool Initialization();

	// DeInitialize component.
	bool DeInitialization();

	// The reference to the current subsystem.
	UPROPERTY()
	TSoftObjectPtr<UModularMoverSubsystem> _subSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
	EDebugMode DebugMode = EDebugMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
	TMap<FName,FDebugItem> PerNameDebugItem;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void AsyncPhysicsTickComponent(float DeltaTime, float SimTime) override;

	// Check movement diff and trigger on move events
	void BeginStateUpdate(FBodyInstance* Body, const FCollisionShape Shape, const FMomentum& Momentum, const FMoverInputPool InputPool, float DeltaTime);

	// Evaluate continuous and instantaneous movement modes as well as Traversals
	void CompleteStateUpdate(const FMoverRequest Request, TArray<FMoverHitResult> MoverHits);

	// Move component from selected movement modes.
	void UpdateMovement(const FBodyInstance* Body, const FMomentum& Momentum, const float DeltaTime);

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
	// Called when a scan surface is done.
	UPROPERTY(BlueprintAssignable, Category="Events|Physic")
	FOnSurfaceCheckSignature OnSurfaceCheck;

	// Disable collision with the world and other characters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	bool bDisableCollision = false;

	// The list of components to ignore when moving.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	TArray<UPrimitiveComponent*> IgnoredCollisionComponents;

	// Use complex collision for movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	bool bUseComplexCollision = false;

protected:
	FVector _lastLocation;
	FVector _lastLocation_trigger;
	FQuat _lastRotation;
	FQuat _lastRotation_trigger;
	FTraceDelegate _onMainSurfaceChk;
	FTraceDelegate _onOffsetChk;
	TMap<FTraceHandle, FMoverRequest> _offsetRequestMap;
	TMap<FTraceHandle, FMoverRequest> _chkRequestMap;
	TArray<FMoverHitResult> _queryHitsBuffer;
	FVector _lastGravity = FVector(0, 0, -1);
	bool _bMoveDisableCollision = false;


	// Get the current Surface.
	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	FSurface GetCurrentSurface() const;

	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	bool IsIgnoringCollision() const;


	//
	void AsyncSweep(UWorld* World, FBodyInstance* Body, const FCollisionShape& Shape, const FVector& ScanVector, const float& DeltaTime, FCollisionQueryParams&
	                QueryParams,
	                const FMomentum& Momentum, const FMoverInputPool& Inputs);

	//
	void OffsetSweep(UWorld* World, FBodyInstance* Body, const FCollisionShape& Shape, const FVector ScanOffset, const FVector& ScanVector, const float& DeltaTime,
	                 FCollisionQueryParams&
	                 QueryParams, const FMomentum& Momentum, const FMoverInputPool& Inputs);
	
	// Called when we've done the surface check trace async
	void QueryResult(const FTraceHandle& TraceHandle, FTraceDatum& TraceData);
	
	// Called when we've done the offset check trace async
	void OffsetResult(const FTraceHandle& TraceHandle, FTraceDatum& TraceData);


	// Calculate Linear velocity. UseReduction subtract the current Linear Velocity from the end result.
	static FVector ComputeLinearVelocity(const FLinearMechanic AttemptedMovement, const FVector currentLinearVelocity, const FVector SurfacesLinearVelocity, const float deltaTime,
	                                     bool UseReduction = false);

	// Calculate Angular velocity (Rad/s). UseReduction subtract the current Angular Velocity from the end result.
	static FVector ComputeAngularVelocity(FAngularMechanic AttemptedMovement, FVector CurrentAngularVelocity, const FQuat CurrentOrientation, const FQuat SurfaceAngularVelocity,
	                                      FVector Gravity,
	                                      const float deltaTime, bool UseReduction = false, const FRotator OffsetRotation = FRotator(0));

#pragma endregion

#pragma region Movement Modes


public:
	// Custom Properties. help when checking move validity. can represent anything, from bool, values, vector, rotation, etc...
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, category = "Movement Modes")
	TMap<FName, FVector> CustomProperties;

	// The currently active Continuous move mode.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, category = "Movement Modes|Continuous")
	FMoverModeSelection ActiveContinuousMove;

	// The currently active Temporary move mode.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, category = "Movement Modes|Temporary")
	FMoverModeSelection ActiveTemporaryMove;

	// Called when the current Continuous move changed
	UPROPERTY(BlueprintAssignable, Category="Events|Movement Modes")
	FOnContinuousChangedSignature OnContinuousMoveChanged;

	// Called when the current Temporary move changed
	UPROPERTY(BlueprintAssignable, Category="Events|Movement Modes")
	FOnTemporaryChangedSignature OnTemporaryMoveChanged;

protected:
	TQueue<FVector> _chkContinuousQueue;
	TQueue<FVector> _chkTemporaryQueue;


	// Continuouss ------------------------------------------------------------------------------------------------------------------------------------------
public:
	// The Continuous move modes used on this controller by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Movement Modes|Continuous")
	TArray<TSubclassOf<UBaseContinuousMove>> ContinuousMoveClasses;

	// The states of each Continuous move for this mover
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, category = "Movement Modes|Continuous")
	TArray<FContinuousMoveInfos> ContinuousMoveState;


	// Add a Continuous move mode
	UFUNCTION(BlueprintCallable, Category="Mover|Movement Modes|Continuous", meta=(NotBlueprintThreadSafe))
	void AddContinuousMoveMode(TSubclassOf<UBaseContinuousMove> Class);

	// Remove a Continuous move mode
	UFUNCTION(BlueprintCallable, Category="Mover|Movement Modes|Continuous", meta=(NotBlueprintThreadSafe))
	void RemoveContinuousMoveMode(FName MoveName);


	// Check Continuous moves and select active Continuous mode.
	void CheckContinuousMoves(const FMoverRequest Request, const TArray<FMoverHitResult> SurfacesHits);


	// Process and blend actives Continuous movements in an async manner (transitions from mode to mode are done at different speeds)
	FMechanicProperties ProcessContinuousMoves(const FMomentum currentMomentum, const FTransform SurfacesMovement, const float DeltaTime);

	// Temporarys ------------------------------------------------------------------------------------------------------------------------------------------

public:
	// The Temporary move modes used on this controller by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Movement Modes|Temporary")
	TArray<TSubclassOf<UBaseTemporaryMove>> TemporaryMoveClasses;

	// The states of each Temporary move for this mover
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, category = "Movement Modes|Temporary")
	TArray<FTemporaryMoveInfos> TemporaryMoveState;


	// Add a Temporary move mode
	UFUNCTION(BlueprintCallable, Category="Mover|Movement Modes|Temporary", meta=(NotBlueprintThreadSafe))
	void AddTemporaryMoveMode(TSubclassOf<UBaseTemporaryMove> Class);

	// Remove a Temporary move mode
	UFUNCTION(BlueprintCallable, Category="Mover|Movement Modes|Temporary", meta=(NotBlueprintThreadSafe))
	void RemoveTemporaryMoveMode(FName MoveName);


	// Check Temporary moves and select active Temporary mode.
	void CheckTemporaryMoves(const FMoverRequest Request, const TArray<FExpandedHitResult> Surfaces);

	// Process and blend actives Temporary movements in a sync manner (transitions from mode to mode are done at the same speed)
	FMechanicProperties ProcessTemporaryMoves(const FMechanicProperties ContinuousMoveResult, const FMomentum currentMomentum, const FTransform SurfacesMovement, const float DeltaTime);

	// Traversal ------------------------------------------------------------------------------------------------------------------------------------------

#pragma endregion

#pragma region Flow Mode

	// Stand alone mode

#pragma endregion

#pragma region Movement

public:
	// The component velocity before friction.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	FVector DesiredVelocity;


	// Move a body according to Movement
	void MoveBody(const FBodyInstance* Body, const FMechanicProperties Movement, const float DeltaTime) const;

	// Get the orientation, keeping body upright.
	static bool GetAngularOrientation(FQuat& Orientation, const FQuat BodyOrientation, const FAngularMechanic angularMechanic, const FVector Gravity,
	                                  const FRotator OffsetRotation = FRotator(0));

	// Calculate an Angular Velocity (Rad/s) to fit orientation, keeping body upright.
	static FVector GetAngularVelocity(const FQuat BodyOrientation, const FAngularMechanic angularMechanic, const FVector Gravity, const FRotator OffsetRotation = FRotator(0));

	// Get the trajectory
	UFUNCTION(BlueprintPure, Category="Mover|Movement")
	static TArray<FMomentum> GetTrajectory(const int SampleCount, const FMechanicProperties inputMovement, const FMomentum currentMomentum, const float timeStep,
	                                       const FRotator OffsetRotation, UModularMoverSubsystem* SubSystem = nullptr);

#pragma endregion
};
