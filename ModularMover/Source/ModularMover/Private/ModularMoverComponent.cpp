// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "ModularMoverComponent.h"
#include "ConversionToolbox.h"
#include "DebugToolbox.h"
#include "PhysicToolbox.h"
#include "Async/Async.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "PhysicsField/PhysicsFieldComponent.h"
#include "HAL/ThreadManager.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UModularMoverComponent::UModularMoverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoRegisterUpdatedComponent = true;
	SetTickGroup(TG_PostPhysics);
	SetAsyncPhysicsTickEnabled(true);
	// ...
	_onMainSurfaceChk.BindUObject(this, &UModularMoverComponent::QueryResult);
	_onOffsetChk.BindUObject(this, &UModularMoverComponent::OffsetResult);
}


// Called when the game starts
void UModularMoverComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Initialization())
	{
		if (!UpdatedPrimitive)
		{
			if (!UpdatedComponent)
				UpdatedComponent = GetOwner()->GetRootComponent();
			if (UpdatedComponent)
				UpdatedPrimitive = Cast<UPrimitiveComponent>(UpdatedComponent);
		}
		if (UpdatedPrimitive)
		{
			UpdatedPrimitive->SetSimulatePhysics(true);
			UpdatedPrimitive->SetUseCCD(true);
			UpdatedPrimitive->SetPhysMaterialOverride(_subSystem->GetStdPhysicMaterial());
			UpdatedPrimitive->SetEnableGravity(false);
			UpdatedPrimitive->SetUpdateKinematicFromSimulation(true);
			UpdatedPrimitive->SetAngularDamping(0);
			UpdatedPrimitive->SetLinearDamping(0);
			UpdatedPrimitive->SetPhysicsMaxAngularVelocityInRadians(FMath::DegreesToRadians(36000), false);
			UpdatedPrimitive->GetBodyInstance()->SetSmoothEdgeCollisionsEnabled(true);
		}
	}
	// ...
}

void UModularMoverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeInitialization();
	Super::EndPlay(EndPlayReason);
}

bool UModularMoverComponent::Initialization()
{
	if (_subSystem)
		return true;
	if (!GetWorld())
		return false;
	_subSystem = GetWorld()->GetSubsystem<UModularMoverSubsystem>();
	if (!_subSystem)
		return false;

	for (int i = 0; i < ContinuousMoveClasses.Num(); i++)
		AddContinuousMoveMode(ContinuousMoveClasses[i]);

	for (int i = 0; i < TemporaryMoveClasses.Num(); i++)
		AddTemporaryMoveMode(TemporaryMoveClasses[i]);

	return true;
}

bool UModularMoverComponent::DeInitialization()
{
	if (!_subSystem)
		return false;
	for (int i = 0; i < ContinuousMoveState.Num(); i++)
		RemoveContinuousMoveMode(ContinuousMoveState[i].BaseInfos.ModeName);

	for (int i = 0; i < ContinuousMoveState.Num(); i++)
		RemoveTemporaryMoveMode(ContinuousMoveState[i].BaseInfos.ModeName);

	return true;
}


// Called every frame
void UModularMoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!_subSystem)
		return;
	// ...
	UpdateInputs(DeltaTime);
	if (UpdatedPrimitive)
	{
		UpdatedPrimitive->SetCollisionEnabled(IsIgnoringCollision() ? ECollisionEnabled::QueryAndProbe : ECollisionEnabled::QueryAndPhysics);
	}
}


// Called every physic sim frame
void UModularMoverComponent::AsyncPhysicsTickComponent(float DeltaTime, float SimTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("AsyncPhysicsTickComponent");
	if (!_subSystem)
		return;
	if (UpdatedPrimitive)
	{
		if (const auto BodyInstance = UpdatedPrimitive->GetBodyInstance())
		{
			const uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
			const FString ThreadName = FThreadManager::Get().GetThreadName(ThreadId);
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AsyncPhysicsTickComponent at %f FPS from %s"), UConversionToolbox::DeltaTimeToFPS(DeltaTime), *ThreadName)
			                                  , true, false, FColor::Red, 0, "ModeName");
			
			//Building Momentum
			FMomentum currentMomentum;
			currentMomentum.Transform = UPhysicToolbox::GetRigidBodyTransform(BodyInstance);
			currentMomentum.LinearVelocity = UPhysicToolbox::GetRigidBodyLinearVelocity(BodyInstance);
			currentMomentum.AngularVelocity = UPhysicToolbox::GetRigidBodyAngularVelocity(BodyInstance);
			currentMomentum.Mass = BodyInstance->GetBodyMass();
			currentMomentum.Gravity = _lastGravity;
			
			// Async check Move Modes
			BeginStateUpdate(BodyInstance, UpdatedPrimitive->GetCollisionShape(-COMPONENT_MAIN_INFLATION), currentMomentum , _inputPool, DeltaTime);
			
			// Process and Move
			UpdateMovement(BodyInstance, currentMomentum, DeltaTime);
		}
	}
	// ...
	Super::AsyncPhysicsTickComponent(DeltaTime, SimTime);
}


void UModularMoverComponent::BeginStateUpdate(FBodyInstance* Body, const FCollisionShape Shape, const FMomentum& Momentum, const FMoverInputPool InputPool, float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("BeginStateUpdate");
	if (!_subSystem || !Body || !GetWorld())
		return;

	//Make request
	const auto world = GetWorld();

	for (int i = 0; i < ContinuousMoveState.Num(); i++)
	{
		if (const auto CMovement = _subSystem->GetContinuousMoveObject(ContinuousMoveState[i].BaseInfos.ModeName))
		{
			const FVector scanDirection = CMovement->ScanSurfaceVector;
			const float scanOffset = CMovement->ScanSurfaceOffset;
			FVector offset = -scanDirection.GetSafeNormal() * scanOffset + Momentum.LinearVelocity.GetSafeNormal()
				* (COMPONENT_MAIN_INFLATION + Momentum.LinearVelocity.Length() * 2 * DeltaTime);

			FCollisionQueryParams query = FCollisionQueryParams::DefaultQueryParam;
			query.OwnerTag = ContinuousMoveState[i].BaseInfos.ModeName;
			query.AddIgnoredActor(GetOwner());
			query.AddIgnoredComponents(IgnoredCollisionComponents);
			if (offset.Length() > 0)
			{
				OffsetSweep(world, Body, Shape, offset, scanDirection, DeltaTime, query, Momentum, InputPool);
			}
			else
			{
				AsyncSweep(world, Body, Shape, scanDirection, DeltaTime, query, Momentum, InputPool);
			}
		}
	}
}


void UModularMoverComponent::CompleteStateUpdate(const FMoverRequest Request, TArray<FMoverHitResult> MoverHits)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("CompleteStateUpdate");
	if (!_subSystem)
		return;

	for (int i = 0; i < MoverHits.Num(); i++)
	{
		// Track Surface
		_subSystem->AddTrackedSurface(MoverHits[i].Hit);

		// Visualize
		//if (DebugMode == EDebugMode::SurfaceDetection)
		{
			FName ownerName = MoverHits[i].OwningMovement;
			FLinearColor debugCol = FColor::Red;
			float radius = 40;
			float thickness = 1;
			if (PerNameDebugItem.Contains(ownerName))
			{
				debugCol = PerNameDebugItem[ownerName].Color;
				radius = PerNameDebugItem[ownerName].Radius;
				thickness = PerNameDebugItem[ownerName].Thickness;
			}
			UDebugToolbox::DrawDebugCircleOnHit(MoverHits[i].Hit, radius, debugCol, 0, thickness);
		}
	}

	// Broadcasting
	OnSurfaceCheck.Broadcast(Request.Momentum, MoverHits);

	// Checks
	CheckContinuousMoves(Request, MoverHits);
	// CheckTemporaryMoves(Request, Hits);
}


void UModularMoverComponent::UpdateMovement(const FBodyInstance* Body, const FMomentum& Momentum, const float DeltaTime)
{
	//Evaluate surface movement transform
	// const FVector surfaceLinear = UStructExtension::GetAverageSurfaceVelocityAt(_subSystem->GetSurfaceVelocities(_currentMomentum.Surfaces), _currentMomentum.Transform.GetLocation(),
	//                                                                             DeltaTime);
	// const FVector surfaceAngular = UStructExtension::GetAverageSurfaceAngularSpeed(_subSystem->GetSurfaceVelocities(_currentMomentum.Surfaces));
	// const FQuat surfaceAngularQuat = FQuat(surfaceAngular.GetSafeNormal(), FMath::DegreesToRadians(surfaceAngular.Length()) * DeltaTime);
	// const FTransform surfaceVelocities = FTransform(surfaceAngularQuat, surfaceLinear, FVector::OneVector);

	// Process and blend
	FMechanicProperties move = ProcessContinuousMoves(Momentum, FTransform(), DeltaTime);
	//move = ProcessTemporaryMoves(move, _currentMomentum, surfaceVelocities, DeltaTime);

	//Affect variables
	_bMoveDisableCollision = move.IgnoreCollision;
	_lastGravity = move.Gravity;

	// //Check hit on snap
	// if (move.Linear.SnapDisplacement.SquaredLength() > 0)
	// {
	// 	FHitResult snapHit;
	// 	FCollisionQueryParams queryParams;
	// 	queryParams.AddIgnoredActor(GetOwner());
	// 	queryParams.AddIgnoredComponents(IgnoredCollisionComponents);
	// 	if (GetWorld()->SweepSingleByChannel(snapHit, _currentMomentum.Transform.GetLocation(), _currentMomentum.Transform.GetLocation() + move.Linear.SnapDisplacement
	// 	                                     , _currentMomentum.Transform.GetRotation(), UpdatedPrimitive->GetCollisionObjectType(), UpdatedPrimitive->GetCollisionShape(-1),
	// 	                                     queryParams))
	// 	{
	// 		move.Linear.SnapDisplacement = snapHit.Location - _currentMomentum.Transform.GetLocation();
	// 	}
	// }

	//Move
	MoveBody(Body, move, DeltaTime);
}


#pragma region Inputs


void UModularMoverComponent::MoveComponent(const FVector Movement)
{
	_movementInput = Movement;
}

void UModularMoverComponent::AddVectorInput(const FName InputName, const FVector Vector, const float LifeTime)
{
	if (_inputPool.InputMap.Contains(InputName))
	{
		_inputPool.InputMap[InputName].LifeTime += LifeTime;
		_inputPool.InputMap[InputName].Property = Vector;
	}
	else
	{
		FMoverInput input;
		input.Property = Vector;
		input.Type = EInputType::Vector;
		input.LifeTime = LifeTime;
		_inputPool.InputMap.Add(InputName, input);
	}
}

void UModularMoverComponent::AddRotationInput(const FName InputName, const FRotator Rotation, const float LifeTime)
{
	if (_inputPool.InputMap.Contains(InputName))
	{
		_inputPool.InputMap[InputName].LifeTime += LifeTime;
		_inputPool.InputMap[InputName].Property = FVector(Rotation.Pitch, Rotation.Yaw, Rotation.Roll);
	}
	else
	{
		FMoverInput input;
		input.Property = FVector(Rotation.Pitch, Rotation.Yaw, Rotation.Roll);
		input.Type = EInputType::Rotation;
		input.LifeTime = LifeTime;
		_inputPool.InputMap.Add(InputName, input);
	}
}

void UModularMoverComponent::AddValueInput(const FName InputName, const float Value, const float LifeTime)
{
	if (_inputPool.InputMap.Contains(InputName))
	{
		_inputPool.InputMap[InputName].LifeTime += LifeTime;
		_inputPool.InputMap[InputName].Property = FVector(Value, 0, 0);
	}
	else
	{
		FMoverInput input;
		input.Property = FVector(Value, 0, 0);
		input.Type = EInputType::Value;
		input.LifeTime = LifeTime;
		_inputPool.InputMap.Add(InputName, input);
	}
}

void UModularMoverComponent::AddTriggerInput(const FName InputName, const float LifeTime)
{
	if (_inputPool.InputMap.Contains(InputName))
	{
		_inputPool.InputMap[InputName].LifeTime += LifeTime;
		_inputPool.InputMap[InputName].Property = FVector(1, 0, 0);
	}
	else
	{
		FMoverInput input;
		input.Property = FVector(1, 0, 0);
		input.Type = EInputType::Trigger;
		input.LifeTime = LifeTime;
		_inputPool.InputMap.Add(InputName, input);
	}
}

bool UModularMoverComponent::ReadVectorInput(const FName InputName, FVector& OutVector) const
{
	return UStructExtension::ReadVectorInput(_inputPool, InputName, OutVector);
}

bool UModularMoverComponent::ReadRotationInput(const FName InputName, FRotator& OutRotation) const
{
	return UStructExtension::ReadRotationInput(_inputPool, InputName, OutRotation);
}

bool UModularMoverComponent::ReadValueInput(const FName InputName, float& OutValue) const
{
	return UStructExtension::ReadValueInput(_inputPool, InputName, OutValue);
}

bool UModularMoverComponent::ReadTriggerInput(const FName InputName, bool& OutTrigger) const
{
	return UStructExtension::ReadTriggerInput(_inputPool, InputName, OutTrigger);
}

void UModularMoverComponent::UpdateInputs(float deltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("UpdateInputs");
	if (_inputPool.InputMap.Num() <= 0)
		return;
	_inputClearList.Empty();
	FString debugTxt = FString();
	for (auto item : _inputPool.InputMap)
	{
		_inputPool.InputMap[item.Key].LifeTime -= deltaTime;
		//Debug
		debugTxt.Append(FString::Printf(
			TEXT("[Active] - [Name (%s); Type (%s); LifeTime (%f)]\n"), *item.Key.ToString(), *UEnum::GetValueAsName(item.Value.Type).ToString(),
			_inputPool.InputMap[item.Key].LifeTime));
		if (_inputPool.InputMap[item.Key].LifeTime <= 0)
			_inputClearList.Add(item.Key);
	}
	if (_inputClearList.Num() > 0)
	{
		for (auto entry : _inputClearList)
			_inputPool.InputMap.Remove(entry);
		_inputClearList.Empty();
	}

	if (DebugMode == EDebugMode::Inputs)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
			                                  TEXT("{Inputs}--------------------------------\n%s-------------------------"), *debugTxt), true,
		                                  false, FColor::Magenta, 60, "InputsUpdate");
	}
}


#pragma endregion


#pragma region Physic

FSurface UModularMoverComponent::GetCurrentSurface() const
{
	return ActiveContinuousMove.Surface;
}

bool UModularMoverComponent::IsIgnoringCollision() const
{
	bool ignore = bDisableCollision;
	if (!ignore)
	{
		ignore = _bMoveDisableCollision;
	}
	return ignore;
}


void UModularMoverComponent::AsyncSweep(UWorld* World, FBodyInstance* Body, const FCollisionShape& Shape, const FVector& ScanVector,
                                        const float& DeltaTime, FCollisionQueryParams&
                                        QueryParams, const FMomentum& Momentum, const FMoverInputPool& Inputs)
{
	if (!Body || !World)
		return;
	const auto rq = UPhysicToolbox::AsyncComponentTraceMulti_internal(World, Shape, Momentum.Transform.GetLocation(), ScanVector, Momentum.Transform.GetRotation(), &_onMainSurfaceChk,
	                                                                  bUseComplexCollision, QueryParams);
	if (_chkRequestMap.Contains(rq))
		_chkRequestMap[rq] = FMoverRequest(Body, Momentum, Inputs, DeltaTime, ScanVector);
	else
		_chkRequestMap.Add(rq, FMoverRequest(Body, Momentum, Inputs, DeltaTime, ScanVector));
}

void UModularMoverComponent::OffsetSweep(UWorld* World, FBodyInstance* Body, const FCollisionShape& Shape, const FVector ScanOffset, const FVector& ScanVector,
                                         const float& DeltaTime, FCollisionQueryParams& QueryParams, const FMomentum& Momentum, const FMoverInputPool& Inputs)
{
	if (!Body || !World)
		return;

	QueryParams.bTraceComplex = bUseComplexCollision;
	QueryParams.bReturnPhysicalMaterial = true;

	const FVector endPoint = Momentum.Transform.GetLocation() + ScanOffset;
	const FCollisionObjectQueryParams objQuery = FCollisionObjectQueryParams::AllObjects;
	const auto rq = World->AsyncSweepByObjectType(EAsyncTraceType::Single, Momentum.Transform.GetLocation(), endPoint, Momentum.Transform.GetRotation(), objQuery, Shape, QueryParams,
	                                              &_onOffsetChk);
	if (_offsetRequestMap.Contains(rq))
		_offsetRequestMap[rq] = FMoverRequest(Body, Momentum, Inputs, DeltaTime, ScanVector);
	else
		_offsetRequestMap.Add(rq, FMoverRequest(Body, Momentum, Inputs, DeltaTime, ScanVector));
}

void UModularMoverComponent::QueryResult(const FTraceHandle& TraceHandle, FTraceDatum& TraceData)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("QueryResult");
	if (!_chkRequestMap.Contains(TraceHandle))
		return;
	const FMoverRequest request = _chkRequestMap[TraceHandle];
	_chkRequestMap.Remove(TraceHandle);
	_queryHitsBuffer.Empty();
	for (const auto hit : TraceData.OutHits)
		_queryHitsBuffer.Add(FMoverHitResult(hit, TraceData.CollisionParams.CollisionQueryParam.OwnerTag, TraceData.CollisionParams.CollisionShape));
	CompleteStateUpdate(request, _queryHitsBuffer);
}

void UModularMoverComponent::OffsetResult(const FTraceHandle& TraceHandle, FTraceDatum& TraceData)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("OffsetResult");
	if (!_offsetRequestMap.Contains(TraceHandle))
		return;
	const FMoverRequest request = _offsetRequestMap[TraceHandle];
	_offsetRequestMap.Remove(TraceHandle);

	FVector newLocation = TraceData.End;
	FVector newOffset = TraceData.End - TraceData.Start;
	if (TraceData.OutHits.Num() > 0 && TraceData.OutHits[0].IsValidBlockingHit())
	{
		newLocation = TraceData.OutHits[0].Location - newOffset.GetSafeNormal() * 0.5;
		newOffset = newLocation - request.Momentum.Transform.GetLocation();
	}
	auto momentum = request.Momentum;
	momentum.Transform.SetLocation(newLocation);
	FCollisionQueryParams query = TraceData.CollisionParams.CollisionQueryParam;
	AsyncSweep(TraceData.PhysWorld.Get(), request.MoverBody, TraceData.CollisionParams.CollisionShape,
	           request.ScanDirection - newOffset.ProjectOnToNormal(request.ScanDirection.GetSafeNormal()), request.DeltaTime, query, momentum, request.InputPool);
}


#pragma endregion


#pragma region Movement Mode

void UModularMoverComponent::AddContinuousMoveMode(TSubclassOf<UBaseContinuousMove> Class)
{
	if (!_subSystem)
		return;
	FContinuousMoveInfos NewMoveInfos;
	if (_subSystem->AddContinuousLibrary(NewMoveInfos, Class))
	{
		int index = -1;
		if (ContinuousMoveState.Num() > 0)
		{
			index = ContinuousMoveState.IndexOfByPredicate([NewMoveInfos](const FContinuousMoveInfos& info)-> bool
			{
				return info.BaseInfos.ModeName == NewMoveInfos.BaseInfos.ModeName;
			});
		}
		if (!ContinuousMoveState.IsValidIndex(index))
		{
			ContinuousMoveState.Add(NewMoveInfos);
		}
	}
}

void UModularMoverComponent::RemoveContinuousMoveMode(FName MoveName)
{
	if (!_subSystem)
		return;
	if (_subSystem->RemoveContinuousLibrary(MoveName))
	{
		if (ContinuousMoveState.Num() <= 0)
			return;
		const int index = ContinuousMoveState.IndexOfByPredicate([MoveName](const FContinuousMoveInfos& infos)-> bool
		{
			return infos.BaseInfos.ModeName == MoveName;
		});
		if (ContinuousMoveState.IsValidIndex(index))
		{
			ContinuousMoveState.RemoveAt(index);
		}
	}
}

void UModularMoverComponent::CheckContinuousMoves(const FMoverRequest Request, const TArray<FMoverHitResult> SurfacesHits)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("CheckContinuousMoves");
	if (!_subSystem)
		return;
	_chkContinuousQueue.Empty();
	for (int i = 0; i < ContinuousMoveState.Num(); i++)
	{
		//check
		if (const auto MovementMode = _subSystem->GetContinuousMoveObject(ContinuousMoveState[i].BaseInfos.ModeName))
		{
			int surfacesIndexesFlag = 0;
			if (MovementMode->CheckContinuousMovement(this, SurfacesHits, ContinuousMoveState[i], Request.Momentum, _movementInput, Request.InputPool, ContinuousMoveState,
			                                          TemporaryMoveState,
			                                          CustomProperties, surfacesIndexesFlag))
			{
				//enqueue
				_chkContinuousQueue.Enqueue(FVector(i, ContinuousMoveState[i].BaseInfos.Priority, surfacesIndexesFlag));
			}
		}
	}

	int masterPriority = -1;
	int masterIndex = -1;
	int masterSurfaceFlag = 0;
	FVector item;
	while (_chkContinuousQueue.Dequeue(item))
	{
		//check priority
		if (item.Y <= masterPriority)
			continue;
		//set index
		masterIndex = item.X;
		//set high priority
		masterPriority = item.Y;
		//set surface flag
		masterSurfaceFlag = item.Z;
	}

	if (!ContinuousMoveState.IsValidIndex(masterIndex))
		return;

	//check changes in active
	if (ActiveContinuousMove.IsValid() && ActiveContinuousMove.ModeName == ContinuousMoveState[masterIndex].BaseInfos.ModeName)
	{
		// auto surfaceActives = UConversionToolbox::FlagToBoolArray(masterSurfaceFlag);
		// for (int i = 0; i < surfaceActives.Num(); i++)
		// {
		// 	if (!SurfacesHits.IsValidIndex(i))
		// 		continue;
		// 	if (!surfaceActives[i])
		// 		continue;
		// 	if (ActiveContinuousMove.Surface.HitResult.Component == SurfacesHits[i].HitResult.Component
		// 		&& FMath::IsNearlyEqual(ActiveContinuousMove.Surface.HitDepth, SurfacesHits[i].SurfaceTraceDepth, 10))
		// 	{
		// 		ActiveContinuousMove.Surface.UpdateHit(Request.MoverBody, SurfacesHits[i].HitResult, SurfacesHits[i].OffsetType);
		// 		break;
		// 	}
		// }
		return;
	}
	// Set new active
	if (const auto MovementMode = _subSystem->GetContinuousMoveObject(ContinuousMoveState[masterIndex].BaseInfos.ModeName))
	{
		FMoverModeSelection selection = FMoverModeSelection();
		// auto surfaceActives = UConversionToolbox::FlagToBoolArray(masterSurfaceFlag);
		// for (int i = 0; i < surfaceActives.Num(); i++)
		// {
		// 	if (!SurfacesHits.IsValidIndex(i))
		// 		continue;
		// 	if (!surfaceActives[i])
		// 		continue;
		// 	selection.Surfaces.Add(FSurface(Request.MoverBody, SurfacesHits[i].HitResult, SurfacesHits[i].OffsetType, SurfacesHits[i].SurfaceTraceDepth));
		// }
		selection.ScanSurfaceOffset = MovementMode->ScanSurfaceOffset;
		selection.ScanSurfaceVector = MovementMode->ScanSurfaceVector;
		selection.ModeName = ContinuousMoveState[masterIndex].BaseInfos.ModeName;
		auto lastActive = ActiveContinuousMove;
		ActiveContinuousMove = selection;

		OnContinuousMoveChanged.Broadcast(lastActive, ActiveContinuousMove);
	}
}

FMechanicProperties UModularMoverComponent::ProcessContinuousMoves(const FMomentum currentMomentum, const FTransform SurfacesMovement, const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ProcessContinuousMoves");
	FMechanicProperties move = FMechanicProperties();
	move.SurfacesMovement = SurfacesMovement;
	FString inactiveDebug;
	float activeWeight = 0;
	int activeIndex = -1;

	//Phase 1
	for (int i = 0; i < ContinuousMoveState.Num(); i++)
	{
		if (ActiveContinuousMove.IsValid() && ContinuousMoveState[i].BaseInfos.ModeName == ActiveContinuousMove.ModeName)
		{
			ActiveContinuousMove.bConsumedActivation = true;

			//Update blends
			ContinuousMoveState[i].BaseInfos.CurrentWeight = FMath::FInterpConstantTo(ContinuousMoveState[i].BaseInfos.CurrentWeight, 1, DeltaTime,
			                                                                          ContinuousMoveState[i].BaseInfos.BlendSpeed.X);
			activeWeight = ContinuousMoveState[i].BaseInfos.CurrentWeight;
			activeIndex = i;
			// Update infos status
			ContinuousMoveState[i].BaseInfos.bIsActiveMode = true;
			ContinuousMoveState[i].BaseInfos.TimeOnMode += DeltaTime;
			ContinuousMoveState[i].TotalTimeOnMode += DeltaTime;
		}
		else
		{
			//Update blends
			ContinuousMoveState[i].BaseInfos.CurrentWeight = FMath::FInterpConstantTo(ContinuousMoveState[i].BaseInfos.CurrentWeight, 0, DeltaTime,
			                                                                          ContinuousMoveState[i].BaseInfos.BlendSpeed.Y);
			// Update infos status
			ContinuousMoveState[i].BaseInfos.bIsActiveMode = false;
			ContinuousMoveState[i].BaseInfos.TimeOnMode = 0;
		}
	}

	if (_subSystem)
	{
		//Phase 2
		if (ContinuousMoveState.IsValidIndex(activeIndex))
		{
			if (const auto MovementMode = _subSystem->GetContinuousMoveObject(ContinuousMoveState[activeIndex].BaseInfos.ModeName))
			{
				// Process
				const auto name = ContinuousMoveState[activeIndex].BaseInfos.ModeName;
				move = MovementMode->ProcessContinuousMovement(this, ContinuousMoveState[activeIndex], currentMomentum, _movementInput, _inputPool, SurfacesMovement, DeltaTime);
				ContinuousMoveState[activeIndex].BaseInfos.ModeName = name;
			}
		}

		//Phase 3
		float blendLeft = 1 - activeWeight;
		inactiveDebug = FString();
		if (blendLeft > 0)
		{
			for (int i = 0; i < ContinuousMoveState.Num(); i++)
			{
				if (blendLeft <= 0)
					break;
				if (i == activeIndex)
					continue;
				if (ContinuousMoveState[i].BaseInfos.CurrentWeight <= 0)
					continue;
				//Debug
				inactiveDebug.Append(FString::Printf(
					TEXT("[Inactive] - [Index (%d); Weight (%f); Name (%s)]\n"), i, ContinuousMoveState[i].BaseInfos.CurrentWeight,
					*ContinuousMoveState[i].BaseInfos.ModeName.ToString()));
			}
		}
	}

	if (DebugMode == EDebugMode::MovementProcessing)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
			                                  TEXT("{Continuous Moves}-------------------------------- \n[Active] - [Index (%d); Weight (%f); Name (%s)]\n%s-------------------------"),
			                                  activeIndex, activeWeight, *ActiveContinuousMove.ModeName.ToString(), *inactiveDebug), true,
		                                  false, FColor::Magenta, 60, "ContinuousMoveInfos");
	}

	return move;
}

void UModularMoverComponent::AddTemporaryMoveMode(TSubclassOf<UBaseTemporaryMove> Class)
{
	if (!_subSystem)
		return;
	FTemporaryMoveInfos NewMoveInfos;
	if (_subSystem->AddTemporaryLibrary(NewMoveInfos, Class))
	{
		int index = -1;
		if (TemporaryMoveState.Num() > 0)
		{
			index = TemporaryMoveState.IndexOfByPredicate([NewMoveInfos](const FTemporaryMoveInfos& info)-> bool
			{
				return info.BaseInfos.ModeName == NewMoveInfos.BaseInfos.ModeName;
			});
		}
		if (!TemporaryMoveState.IsValidIndex(index))
		{
			TemporaryMoveState.Add(NewMoveInfos);
		}
	}
}

void UModularMoverComponent::RemoveTemporaryMoveMode(FName MoveName)
{
	if (!_subSystem)
		return;
	if (_subSystem->RemoveTemporaryLibrary(MoveName))
	{
		if (TemporaryMoveState.Num() <= 0)
			return;
		const int index = TemporaryMoveState.IndexOfByPredicate([MoveName](const FTemporaryMoveInfos& infos)-> bool
		{
			return infos.BaseInfos.ModeName == MoveName;
		});
		if (TemporaryMoveState.IsValidIndex(index))
		{
			TemporaryMoveState.RemoveAt(index);
		}
	}
}

void UModularMoverComponent::CheckTemporaryMoves(const FMoverRequest Request, const TArray<FExpandedHitResult> Surfaces)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("CheckTemporaryMoves");
	if (!_subSystem)
		return;
	_chkTemporaryQueue.Empty();
	for (int i = 0; i < TemporaryMoveState.Num(); i++)
	{
		//check
		if (const auto MovementMode = _subSystem->GetTemporaryMoveObject(TemporaryMoveState[i].BaseInfos.ModeName))
		{
			if (MovementMode->CheckTemporaryMovement(this, Surfaces, TemporaryMoveState[i], Request.Momentum, _movementInput, Request.InputPool, ContinuousMoveState, TemporaryMoveState,
			                                         CustomProperties))
			{
				//enqueue
				_chkTemporaryQueue.Enqueue(FVector(i, TemporaryMoveState[i].BaseInfos.Priority, 0));
			}
		}
	}

	int masterPriority = -1;
	int masterIndex = -1;
	FVector item;
	while (_chkTemporaryQueue.Dequeue(item))
	{
		//check priority
		if (item.Y < masterPriority)
			continue;
		//set index
		masterIndex = item.X;
		//set high priority
		masterPriority = item.Y;
	}

	if (!TemporaryMoveState.IsValidIndex(masterIndex))
		return;

	// Set new active
	if (const auto MovementMode = _subSystem->GetTemporaryMoveObject(TemporaryMoveState[masterIndex].BaseInfos.ModeName))
	{
		FMoverModeSelection selection = FMoverModeSelection();
		selection.ModeName = TemporaryMoveState[masterIndex].BaseInfos.ModeName;
		auto lastActive = ActiveTemporaryMove;
		ActiveTemporaryMove = selection;

		OnTemporaryMoveChanged.Broadcast(lastActive, ActiveTemporaryMove);
	}
}

FMechanicProperties UModularMoverComponent::ProcessTemporaryMoves(const FMechanicProperties ContinuousMoveResult, const FMomentum currentMomentum, const FTransform SurfacesMovement,
                                                                  const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ProcessTemporaryMoves");
	FMechanicProperties move = ContinuousMoveResult;
	FString activeDebug;
	FString inactiveDebug;
	float activeBlendSpeed = 0;
	float activeWeight = 0;
	int activeIndex = ActiveTemporaryMove.IsValid()
		                  ? TemporaryMoveState.IndexOfByPredicate([&](const FTemporaryMoveInfos& infos)-> bool { return infos.BaseInfos.ModeName == ActiveTemporaryMove.ModeName; })
		                  : -1;

	//Phase 1
	if (TemporaryMoveState.IsValidIndex(activeIndex))
	{
		if (!ActiveTemporaryMove.bConsumedActivation)
		{
			//Restarted
			TemporaryMoveState[activeIndex].BaseInfos.bIsActiveMode = false;
			TemporaryMoveState[activeIndex].BaseInfos.TimeOnMode = 0;
			TemporaryMoveState[activeIndex].UpdatePhase();
		}
		ActiveTemporaryMove.bConsumedActivation = true;

		//Update blends
		if (!TemporaryMoveState[activeIndex].BaseInfos.bIsActiveMode)
			TemporaryMoveState[activeIndex].BaseInfos.CurrentWeight = 0;
		activeBlendSpeed = TemporaryMoveState[activeIndex].BaseInfos.BlendSpeed.X;
		TemporaryMoveState[activeIndex].BaseInfos.CurrentWeight = FMath::FInterpConstantTo(TemporaryMoveState[activeIndex].BaseInfos.CurrentWeight, 1, DeltaTime, activeBlendSpeed);
		activeWeight = TemporaryMoveState[activeIndex].BaseInfos.CurrentWeight;

		// Update infos status
		TemporaryMoveState[activeIndex].BaseInfos.bIsActiveMode = true;
		TemporaryMoveState[activeIndex].BaseInfos.TimeOnMode += DeltaTime;
		TemporaryMoveState[activeIndex].UpdatePhase();

		if (_subSystem)
		{
			if (const auto MovementMode = _subSystem->GetTemporaryMoveObject(TemporaryMoveState[activeIndex].BaseInfos.ModeName))
			{
				// Process
				const auto name = TemporaryMoveState[activeIndex].BaseInfos.ModeName;
				move = MovementMode->ProcessTemporaryMovement(this, ContinuousMoveResult, TemporaryMoveState[activeIndex], currentMomentum, _movementInput, _inputPool, SurfacesMovement,
				                                              DeltaTime);
			}
		}

		//Try to disable
		const float outDuration = 1 / TemporaryMoveState[activeIndex].BaseInfos.BlendSpeed.Y;
		if (TemporaryMoveState[activeIndex].BaseInfos.TimeOnMode >= FMath::Clamp(TemporaryMoveState[activeIndex].TotalDuration() - outDuration, 0, TNumericLimits<float>::Max()))
			ActiveTemporaryMove = FMoverModeSelection();

		//Debug
		activeDebug = FString::Printf(TEXT("Time (%f), Phase (%s), Lenght (%f), outDuration (%f)"), TemporaryMoveState[activeIndex].BaseInfos.TimeOnMode,
		                              *TemporaryMoveState[activeIndex].CurrentPhase.PhaseName.ToString(), TemporaryMoveState[activeIndex].TotalDuration(), outDuration);
	}

	//Phase 2
	float blendLeft = 1 - activeWeight;
	inactiveDebug = FString();
	for (int i = 0; i < TemporaryMoveState.Num(); i++)
	{
		if (i == activeIndex)
			continue;

		//Update blends
		const float speed = activeBlendSpeed > 0 ? activeBlendSpeed : TemporaryMoveState[i].BaseInfos.BlendSpeed.Y;
		TemporaryMoveState[i].BaseInfos.CurrentWeight = FMath::FInterpConstantTo(TemporaryMoveState[i].BaseInfos.CurrentWeight, 0, DeltaTime, speed);
		// Update infos status
		TemporaryMoveState[i].BaseInfos.bIsActiveMode = false;
		TemporaryMoveState[i].BaseInfos.TimeOnMode = 0;

		if (blendLeft <= 0)
			continue;
		if (TemporaryMoveState[i].BaseInfos.CurrentWeight <= 0)
			continue;

		//Debug
		inactiveDebug.Append(FString::Printf(
			TEXT("[Inactive] - [Index (%d); Weight (%f); Name (%s), Phase (%s), Lenght (%f)]\n"), i, TemporaryMoveState[i].BaseInfos.CurrentWeight,
			*TemporaryMoveState[i].BaseInfos.ModeName.ToString(),
			*TemporaryMoveState[i].CurrentPhase.PhaseName.ToString(), TemporaryMoveState[i].TotalDuration()));
	}

	if (DebugMode == EDebugMode::MovementProcessing)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
			                                  TEXT("{Temporary Moves}-------------------------------- \n[Active] - [Index (%d); Weight (%f); Name (%s) %s]\n%s-------------------------"),
			                                  activeIndex, activeWeight, *ActiveTemporaryMove.ModeName.ToString(), *activeDebug, *inactiveDebug), true,
		                                  false, FColor::Magenta, 60, "TemporaryMoveInfos");
	}

	return move;
}


#pragma endregion


#pragma region Flow Mode

// Stand alone mode

#pragma endregion


#pragma region Movement


FVector UModularMoverComponent::ComputeLinearVelocity(const FLinearMechanic AttemptedMovement, const FVector currentLinearVelocity, const FVector SurfacesLinearVelocity,
                                                      const float deltaTime, bool UseReduction)
{
	const FVector relativeVelocity = AttemptedMovement.Acceleration;
	FVector velocity = SurfacesLinearVelocity + relativeVelocity;

	if (UseReduction)
		velocity -= currentLinearVelocity;

	return velocity;
}

FVector UModularMoverComponent::ComputeAngularVelocity(FAngularMechanic AttemptedMovement, FVector CurrentAngularVelocity, const FQuat CurrentOrientation, const FQuat SurfaceAngularVelocity,
                                                       FVector Gravity,
                                                       const float deltaTime, bool UseReduction, const FRotator OffsetRotation)
{
	if (!Gravity.Normalize())
		Gravity = FVector::DownVector;
	FAngularMechanic angularPart = AttemptedMovement;
	const float rotSpeed = angularPart.LookOrientation.Length();
	if (angularPart.OrientationDiff.IsIdentity() && angularPart.LookOrientation.Normalize())
	{
		if (!FMath::IsNearlyEqual(angularPart.LookOrientation | CurrentOrientation.Vector(), 1, 0.0001))
		{
			const FQuat targetRot = UKismetMathLibrary::MakeRotFromZX(-Gravity.GetSafeNormal(), angularPart.LookOrientation).Quaternion();
			FQuat desiredOrientation = targetRot;
			if (!FMath::IsNearlyEqual(targetRot.Vector() | CurrentOrientation.Vector(), 1, 0.0001))
				desiredOrientation = FMath::QInterpConstantTo(CurrentOrientation, targetRot, deltaTime, FMath::DegreesToRadians(rotSpeed));
			desiredOrientation.EnforceShortestArcWith(CurrentOrientation);
			angularPart.OrientationDiff = desiredOrientation * CurrentOrientation.Inverse();
		}
	}

	angularPart.OrientationDiff *= SurfaceAngularVelocity;
	FVector result = GetAngularVelocity(CurrentOrientation, angularPart, Gravity, OffsetRotation) / deltaTime;

	if (UseReduction)
		result -= CurrentAngularVelocity;

	return result;
}


void UModularMoverComponent::MoveBody(const FBodyInstance* Body, const FMechanicProperties Movement, const float DeltaTime) const
{
	if (!Body)
		return;
	const FTransform initialTransform = UPhysicToolbox::GetRigidBodyTransform(Body);
	const FVector currentLinearVelocity = UPhysicToolbox::GetRigidBodyLinearVelocity(Body);
	const FVector currentAngularVelocity = UPhysicToolbox::GetRigidBodyAngularVelocity(Body);

	//
	FVector linearMovement;
	FVector angularMovement;

	//Linear Part
	{
		linearMovement = ComputeLinearVelocity(Movement.Linear, currentLinearVelocity, Movement.SurfacesMovement.GetLocation(), DeltaTime, true);

		if (DebugMode == EDebugMode::LinearMovement)
		{
			const float moveDirection = FMath::Clamp(currentLinearVelocity.GetSafeNormal() | Movement.Linear.Acceleration.GetSafeNormal(), -1, 1);
			UKismetSystemLibrary::PrintString(this, FString::Printf(
				                                  TEXT("[LinearVelocity] - Current Vel (%f); Move Dir (%f); Movement Vel (%f)"),
				                                  currentLinearVelocity.Length(), moveDirection, linearMovement.Length()), true,
			                                  false, FColor::Magenta, 60, "ComputeLinearVelocity");
		}
	}


	//Angular Part
	{
		angularMovement = ComputeAngularVelocity(Movement.Angular, currentAngularVelocity, initialTransform.GetRotation(), Movement.SurfacesMovement.GetRotation(), Movement.Gravity,
		                                         DeltaTime, true, FRotator::ZeroRotator);

		if (DebugMode == EDebugMode::AngularMovement)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(
				                                  TEXT("[AngularVelocity] - Current Vel (%f); Rotation Vel (%f)"),
				                                  FMath::RadiansToDegrees(currentAngularVelocity.Length()), FMath::RadiansToDegrees(angularMovement.Length())), true,
			                                  false, FColor::Magenta, 60, "ComputeAngularVelocity");
		}
	}

	//Wake up and apply movement
	UPhysicToolbox::RigidBodySetWorldLocation(Body, initialTransform.GetLocation() + Movement.Linear.SnapDisplacement);
	UPhysicToolbox::RigidBodyAddImpulse(Body, linearMovement, true);
	UPhysicToolbox::RigidBodyAddAngularImpulseInRadians(Body, angularMovement, true);
}

bool UModularMoverComponent::GetAngularOrientation(FQuat& Orientation, const FQuat BodyOrientation, const FAngularMechanic angularMechanic, const FVector Gravity,
                                                   const FRotator OffsetRotation)
{
	Orientation = BodyOrientation;
	FVector gravityUp = -Gravity;
	if (!gravityUp.Normalize())
		gravityUp = FVector::UpVector;

	// Apply orientation diff
	Orientation *= angularMechanic.OrientationDiff;

	//OrientationDiff
	{
		FVector virtualFwdDir = FVector::VectorPlaneProject(Orientation.Vector(), gravityUp);
		FVector virtualRightDir = FVector::ZeroVector;
		if (virtualFwdDir.Normalize())
		{
			virtualRightDir = FVector::CrossProduct(gravityUp, virtualFwdDir);
		}
		else
		{
			virtualFwdDir = BodyOrientation.GetAxisX(); // -virtualFwdDir.Rotation().Quaternion().GetAxisZ();
			FVector::CreateOrthonormalBasis(virtualFwdDir, virtualRightDir, gravityUp);
			virtualFwdDir.Normalize();
		}
		if (!virtualRightDir.Normalize())
		{
			return false;
		}
		const FRotator desiredRotator = UKismetMathLibrary::MakeRotFromZX(gravityUp, virtualFwdDir);
		virtualFwdDir = desiredRotator.Quaternion().GetAxisX();
		virtualRightDir = desiredRotator.Quaternion().GetAxisY();

		Orientation = desiredRotator.Quaternion() * OffsetRotation.Quaternion();
	}


	return true;
}

FVector UModularMoverComponent::GetAngularVelocity(const FQuat BodyOrientation, const FAngularMechanic angularMechanic, const FVector Gravity, const FRotator OffsetRotation)
{
	FQuat orientation;
	if (!GetAngularOrientation(orientation, BodyOrientation, angularMechanic, Gravity, OffsetRotation))
		return FVector(0);
	return UPhysicToolbox::OrientationDiffToAngularVelocity(BodyOrientation, orientation);
}

TArray<FMomentum> UModularMoverComponent::GetTrajectory(const int SampleCount, const FMechanicProperties inputMovement, const FMomentum currentMomentum, const float timeStep,
                                                        const FRotator OffsetRotation, UModularMoverSubsystem* SubSystem)
{
	TArray<FMomentum> result;
	const FVector surfaceLinear = FVector(0);
	// SubSystem
	//                               ? UStructExtension::GetAverageSurfaceVelocityAt(SubSystem->GetSurfaceVelocities(currentMomentum.Surfaces), currentMomentum.Transform.GetLocation(),
	//                                                                               timeStep)
	//                               : FVector(0);
	const FVector surfaceAngular = FVector(0);
	//SubSystem ? UStructExtension::GetAverageSurfaceAngularSpeed(SubSystem->GetSurfaceVelocities(currentMomentum.Surfaces)) : FVector(0);
	const FQuat surfaceAngularQuat = FQuat(surfaceAngular.GetSafeNormal(), FMath::DegreesToRadians(surfaceAngular.Length()) * timeStep);

	result.Add(currentMomentum);
	for (int i = 0; i < SampleCount; i++)
	{
		FMomentum lastItem = result[result.Num() - 1];
		FMomentum iteration;
		iteration.Mass = currentMomentum.Mass;

		//Linear part
		{
			iteration.LinearVelocity = ComputeLinearVelocity(inputMovement.Linear, lastItem.LinearVelocity, surfaceLinear, timeStep);
			iteration.Transform.SetLocation(lastItem.Transform.GetLocation() + iteration.LinearVelocity * timeStep);
		}

		//Angular part
		{
			iteration.AngularVelocity = ComputeAngularVelocity(inputMovement.Angular, lastItem.AngularVelocity, lastItem.Transform.GetRotation(), surfaceAngularQuat, inputMovement.Gravity,
			                                                   timeStep, false, OffsetRotation);
			const FQuat axisAngleRot = FQuat(iteration.AngularVelocity.GetSafeNormal(), iteration.AngularVelocity.Length() * timeStep);
			FQuat newRot = lastItem.Transform.GetRotation() * axisAngleRot;
			const float newDot = (newRot.Vector() | inputMovement.Angular.LookOrientation.GetSafeNormal());
			const float oldDot = (lastItem.Transform.GetRotation().Vector() | inputMovement.Angular.LookOrientation.GetSafeNormal());
			if (!inputMovement.Angular.OrientationDiff.IsIdentity() || (newDot != 0 && oldDot != 0 && newDot >= oldDot))
			{
				iteration.Transform.SetRotation(newRot);
			}
			else
			{
				iteration.Transform.SetRotation(lastItem.Transform.GetRotation());
			}
		}

		result.Add(iteration);
	}

	return result;
}

#pragma endregion
