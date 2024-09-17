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
}

UModularMoverComponent::~UModularMoverComponent()
{
	FinalUnregistration();
}


// Called when the game starts
void UModularMoverComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!_subSystem)
		InitialRegistration();
	if (_subSystem)
	{
		if (!UpdatedComponent)
			UpdatedComponent = GetOwner()->GetRootComponent();
		if (UpdatedComponent)
			UpdatedPrimitive = Cast<UPrimitiveComponent>(UpdatedComponent);
		if (UpdatedPrimitive)
		{
			OnComponentMoved.AddUObject(this, &UModularMoverComponent::OnMoveCheck);
			_onMainSurfaceChk.BindUObject(this, &UModularMoverComponent::OnMainSurfaceCheckDone);

			const FTransform curTr = UpdatedPrimitive->GetBodyInstance()->GetUnrealWorldTransform_AssumesLocked();
			const FVector _curLocation = curTr.GetLocation();
			const FQuat _curRotation = curTr.GetRotation();
			_lastLocation = _curLocation;
			_lastRotation = _curRotation;
			_lastLocation_trigger = _curLocation;
			_lastRotation_trigger = _curRotation;

			UpdatedPrimitive->SetSimulatePhysics(true);
			UpdatedPrimitive->SetUseCCD(true);
			_customPhysicMaterial = NewObject<UPhysicalMaterial>();
			_customPhysicMaterial->bOverrideFrictionCombineMode = true;
			_customPhysicMaterial->bOverrideRestitutionCombineMode = true;
			_customPhysicMaterial->Friction = 0;
			_customPhysicMaterial->StaticFriction = 0;
			_customPhysicMaterial->FrictionCombineMode = EFrictionCombineMode::Min;
			_customPhysicMaterial->Restitution = 0;
			_customPhysicMaterial->RestitutionCombineMode = EFrictionCombineMode::Min;
			UpdatedPrimitive->SetPhysMaterialOverride(_customPhysicMaterial);
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

bool UModularMoverComponent::InitialRegistration()
{
	if (!GetWorld())
		return false;
	_subSystem = GetWorld()->GetSubsystem<UModularMoverSubsystem>();
	if (!_subSystem)
		return false;
	_subSystem->RegisterComponent(this);
	return true;
}

bool UModularMoverComponent::FinalUnregistration()
{
	if (!_subSystem)
		return false;
	_subSystem->UnRegisterComponent(this);
	return true;
}


// Called every frame
void UModularMoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
	UpdateInputs(DeltaTime);
	if (UpdatedPrimitive)
		UpdatedPrimitive->SetCollisionEnabled(IsIgnoringCollision() ? ECollisionEnabled::QueryAndProbe : ECollisionEnabled::QueryAndPhysics);
}


// Called every physic sim frame
void UModularMoverComponent::AsyncPhysicsTickComponent(float DeltaTime, float SimTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("AsyncPhysicsTickComponent");
	if (UpdatedPrimitive)
	{
		if (const auto BodyInstance = UpdatedPrimitive->GetBodyInstance())
		{
			const uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
			const FString ThreadName = FThreadManager::Get().GetThreadName(ThreadId);
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AsyncPhysicsTickComponent at %f FPS from %s"), UConversionToolbox::DeltaTimeToFPS(DeltaTime), *ThreadName)
			                                  , true, false, FColor::Red, 0, "ModeName");
			FMomentum currentMomentum;
			currentMomentum.Transform = UPhysicToolbox::GetRigidBodyTransform(BodyInstance);
			currentMomentum.LinearVelocity = UPhysicToolbox::GetRigidBodyLinearVelocity(BodyInstance);
			currentMomentum.AngularVelocity = UPhysicToolbox::GetRigidBodyAngularVelocity(BodyInstance);
			currentMomentum.Mass = GetMass();
			currentMomentum.Gravity = _lastGravity;

			EvaluateMovementDiff(currentMomentum.Transform);

			FMechanicProperties move = FMechanicProperties();
			// TODO: Update Blend Contingent Moves Mechanic Properties
			if (ContingentMoveState.IsValidIndex(0) && _subSystem)
			{
				if (auto MovementMode = _subSystem->GetContingentMoveObject(ContingentMoveState[0].BaseInfos.ModeName))
				{
					move = MovementMode->ProcessMovement(currentMomentum, _movementInput, _inputPool, DeltaTime);
				}
			}
			// TODO: Update Blend transient Moves Mechanic Properties

			_lastGravity = move.Gravity;
			MoveBody(BodyInstance, move, DeltaTime);
		}
	}
	// ...
	Super::AsyncPhysicsTickComponent(DeltaTime, SimTime);
}

void UModularMoverComponent::EvaluateMovementOnSurfaces(const FTransform Transform, TArray<FExpandedHitResult> Surfaces)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("EvaluateMovementOnSurfaces");
	if (DebugMode == EDebugMode::SurfaceDetection)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Transform.GetLocation(), 25, 8);
		UKismetSystemLibrary::DrawDebugArrow(this, Transform.GetLocation(), Transform.GetLocation() + Transform.GetRotation().Vector() * 100, 100, FColor::White, 0, 3);
		for (int i = 0; i < Surfaces.Num(); i++)
		{
			if (Surfaces[i].HitResult.GetComponent())
				UKismetSystemLibrary::DrawDebugSphere(this, Surfaces[i].HitResult.ImpactPoint, 10, 8);
			//UDebugToolbox::DrawDebugCircleOnHit(Surfaces[i].HitResult, 40, FColor::White, Surfaces[i].HitResult.GetComponent()->GetWorld()->DeltaTimeSeconds, 0.5);
		}
	}
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
		input.Type = EInputType::Rotation;
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
		input.Type = EInputType::Rotation;
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
	for (auto item : _inputPool.InputMap)
	{
		_inputPool.InputMap[item.Key].LifeTime -= deltaTime;
		if (_inputPool.InputMap[item.Key].LifeTime <= 0)
			_inputClearList.Add(item.Key);
	}
	if (_inputClearList.Num() > 0)
	{
		for (auto entry : _inputClearList)
			_inputPool.InputMap.Remove(entry);
		_inputClearList.Empty();
	}
}


#pragma endregion


#pragma region Physic


bool UModularMoverComponent::IsIgnoringCollision() const
{
	bool ignore = bDisableCollision;
	// if (!ignore)
	// {
	// 	if (const auto curAction = GetCurrentControllerAction())
	// 	{
	// 		const auto curActionInfos = GetCurrentControllerActionInfos();
	// 		if (curAction->NoCollisionPhases.Num() > 0)
	// 			ignore = curAction->NoCollisionPhases.Contains(curActionInfos.CurrentPhase);
	// 	}
	//
	// 	if (!ignore)
	// 	{
	// 		ignore = _noCollisionOverrideRootMotionCommand.IsValid();
	// 	}
	// }
	return ignore;
}


FVector UModularMoverComponent::GetCurrentScanSurfaceVector() const
{
	return FVector::Zero();
}


float UModularMoverComponent::GetCurrentScanSurfaceOffset() const
{
	return 0;
}


void UModularMoverComponent::EvaluateMovementDiff(const FTransform BodyTransform)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("EvaluateMovementDiff");
	const FVector _curLocation = BodyTransform.GetLocation();
	const FQuat _curRotation = BodyTransform.GetRotation();
	_lastLocation = _curLocation;
	_lastRotation = _curRotation;
	const FQuat diff = (_curRotation.Inverse() * _lastRotation_trigger);
	FVector axis;
	float angle;
	diff.ToAxisAndAngle(axis, angle);
	if ((_curLocation - _lastLocation_trigger).Length() > MoveTriggerTolerance
		|| FMath::RadiansToDegrees(angle) > RotateTriggerTolerance)
	{
		OnComponentMoved.Broadcast(this, BodyTransform);
		_lastLocation_trigger = _curLocation;
		_lastRotation_trigger = _curRotation;
	}

	// Debug
	if (DebugMode == EDebugMode::Spacial)
	{
		const FVector offset = FVector(MOVER_CHUNK_SIZE);
		const FVector min = FVector(CurrentAreaChunk.X, CurrentAreaChunk.Y, CurrentAreaChunk.Z) * MOVER_CHUNK_SIZE;
		const FVector max = min + offset;

		// Define the box parameters
		const FVector BoxCenter = min + (max - min) * 0.5;
		const FVector BoxExtent(offset.X * 0.5, offset.Y * 0.5, offset.Z * 0.5); // Half-size of the box
		const FColor BoxColor = FColor::Yellow;

		// Log the box
		UKismetSystemLibrary::DrawDebugBox(this, BoxCenter, BoxExtent, BoxColor);
	}
}


void UModularMoverComponent::OnMoveCheck(UModularMoverComponent* Mover, FTransform Transform)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("OnMoveCheck");
	if (Mover != this)
		return;
	if (bUseAsyncSurfaceDetection)
	{
		const auto request = AsyncDetectOverlapHits(Transform, &_onMainSurfaceChk, GetCurrentScanSurfaceVector(), GetCurrentScanSurfaceOffset());
		if (_chkRequestMap.Contains(request))
			_chkRequestMap[request] = Transform;
		else
			_chkRequestMap.Add(request, Transform);
		return;
	}

	TArray<FExpandedHitResult> surfaceHits;
	DetectOverlapHits(Transform, surfaceHits, GetCurrentScanSurfaceVector(), GetCurrentScanSurfaceOffset());
	int maxDepth = 0;
	FixOverlapHits(maxDepth, Transform, surfaceHits, [&](FVector location)-> void
	{
		Transform.SetLocation(location);
	});
	EvaluateMovementOnSurfaces(Transform, surfaceHits);
}


void UModularMoverComponent::OnMainSurfaceCheckDone(const FTraceHandle& TraceHandle, FTraceDatum& TraceData)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("OnMainSurfaceCheckDone");
	if (!_chkRequestMap.Contains(TraceHandle))
		return;
	FTransform transform = _chkRequestMap[TraceHandle];
	_chkRequestMap.Remove(TraceHandle);
	TArray<FExpandedHitResult> outHitResults;
	UPhysicToolbox::PostPhysicTrace_internal(TraceData.OutHits, outHitResults, TraceData.TraceChannel, TraceData.CollisionParams.CollisionQueryParam, ESurfaceTraceHitType::MAX,
	                                         MinDepthSurface);
	int maxDepth = 0;
	FixOverlapHits(maxDepth, transform, outHitResults, [&](FVector location)-> void
	{
		transform.SetLocation(location);
	});
	EvaluateMovementOnSurfaces(transform, outHitResults);
}


void UModularMoverComponent::OverlapSolver(int& maxDepth, const FTransform* customTr) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("OverlapSolver");
	if (!UpdatedPrimitive)
		return;
	if (const auto BodyInstance = UpdatedPrimitive->GetBodyInstance())
	{
		const FTransform curTr = customTr ? *customTr : BodyInstance->GetUnrealWorldTransform_AssumesLocked();
		TArray<FExpandedHitResult> hits;
		if (DetectOverlapHits(curTr, hits))
		{
			FixOverlapHits(maxDepth, curTr, hits, [&](FVector location)-> void
			               {
				               FTransform newTr = curTr;
				               newTr.SetLocation(location);
				               BodyInstance->SetBodyTransform(newTr, ETeleportType::TeleportPhysics);
			               }, [&](UPrimitiveComponent* comp, FVector force)-> void
			               {
				               if (comp) comp->AddForce(force * GetMass() / GetWorld()->DeltaTimeSeconds);
			               });
		}
	}
}


bool UModularMoverComponent::DetectOverlapHits(const FTransform Transform, TArray<FExpandedHitResult>& touchedHits, const FVector scanVector, const uint64 CounterDirectionOffset) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("DetectOverlapHits");
	touchedHits.Empty();
	if (!GetWorld() || !UpdatedPrimitive)
		return false;
	const auto world = GetWorld();
	constexpr float detectionInflation = OVERLAP_INFLATION;
	const FVector location = Transform.GetLocation();
	const FQuat rotation = Transform.GetRotation();
	const auto shape = UpdatedPrimitive->GetCollisionShape(detectionInflation);
	const auto channel = UpdatedPrimitive->GetCollisionObjectType();
	FVector offset = -scanVector.GetSafeNormal() * CounterDirectionOffset;
	if (DebugMode == EDebugMode::SurfaceDetection)
	{
		FHitResult hit;
		hit.Normal = scanVector.GetSafeNormal();
		hit.ImpactNormal = scanVector.GetSafeNormal();
		hit.ImpactPoint = location - offset;
		hit.Component = UpdatedPrimitive;
		UDebugToolbox::DrawDebugCircleOnHit(hit, 40, FColor::Silver, world->DeltaTimeSeconds, 0.5);
	}
	FCollisionQueryParams query = FCollisionQueryParams::DefaultQueryParam;
	query.AddIgnoredActor(UpdatedPrimitive->GetOwner());
	query.AddIgnoredComponents(IgnoredCollisionComponents);
	bool hitRes = UPhysicToolbox::ComponentTraceMulti_internal(world, shape, channel, touchedHits, location - offset, scanVector + offset, rotation, bUseComplexCollision,
	                                                           query, ESurfaceTraceHitType::MAX, MinDepthSurface);
	return hitRes;
}


FTraceHandle UModularMoverComponent::AsyncDetectOverlapHits(const FTransform Transform, FTraceDelegate* callBack, const FVector scanVector, const uint64 CounterDirectionOffset) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("AsyncDetectOverlapHits");
	if (!GetWorld() || !UpdatedPrimitive)
		return {};
	const auto world = GetWorld();
	constexpr float detectionInflation = OVERLAP_INFLATION;
	const FVector location = Transform.GetLocation();
	const FQuat rotation = Transform.GetRotation();
	const auto shape = UpdatedPrimitive->GetCollisionShape(detectionInflation);
	const auto channel = UpdatedPrimitive->GetCollisionObjectType();
	FVector offset = -scanVector.GetSafeNormal() * CounterDirectionOffset;
	if (DebugMode == EDebugMode::SurfaceDetection)
	{
		FHitResult hit;
		hit.Normal = scanVector.GetSafeNormal();
		hit.ImpactNormal = scanVector.GetSafeNormal();
		hit.ImpactPoint = location - offset;
		hit.Component = UpdatedPrimitive;
		UDebugToolbox::DrawDebugCircleOnHit(hit, 40, FColor::Silver, world->DeltaTimeSeconds, 0.5);
	}
	FCollisionQueryParams query = FCollisionQueryParams::DefaultQueryParam;
	query.AddIgnoredActor(UpdatedPrimitive->GetOwner());
	query.AddIgnoredComponents(IgnoredCollisionComponents);
	return UPhysicToolbox::AsyncComponentTraceMulti_internal(world, shape, location - offset, scanVector + offset, rotation, callBack, bUseComplexCollision, query);
}


void UModularMoverComponent::FixOverlapHits(int& maxDepth, const FTransform Transform, const TArray<FExpandedHitResult> touchedHits, std::function<void(FVector)> OnLocationSet,
                                            std::function<void(UPrimitiveComponent*, FVector)> OnPhysicCompHit) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("FixOverlapHits");
	if (touchedHits.Num() <= 0)
		return;
	if (!GetWorld() || !UpdatedPrimitive)
		return;
	const auto world = GetWorld();
	FComponentQueryParams comQueryParams;
	if (UpdatedPrimitive->GetOwner())
		comQueryParams.AddIgnoredActor(UpdatedPrimitive->GetOwner());
	const auto shape = UpdatedPrimitive->GetCollisionShape(0);
	const FVector location = Transform.GetLocation();
	const FQuat rotation = Transform.GetRotation();
	const auto channel = UpdatedPrimitive->GetCollisionObjectType();
	FMTDResult penetrationInfos;
	FVector displacement = FVector(0);
	for (int i = 0; i < touchedHits.Num(); i++)
	{
		auto& overlapHit = touchedHits[i];

		const bool isBlocking = overlapHit.CollisionResponse == ECollisionResponse::ECR_Block;
		if (!isBlocking || IsIgnoringCollision())
			continue;
		if (overlapHit.HitResult.GetComponent() && overlapHit.HitResult.GetComponent()->ComputePenetration(penetrationInfos, shape, location, rotation))
		{
			comQueryParams.AddIgnoredComponent(overlapHit.HitResult.GetComponent());
			const FVector depForce = penetrationInfos.Direction * penetrationInfos.Distance;

			//Handle physic objects collision
			if (overlapHit.HitResult.Component->IsSimulatingPhysics())
			{
				if (OnPhysicCompHit != nullptr)
					OnPhysicCompHit(overlapHit.HitResult.GetComponent(), -depForce);
			}

			displacement += depForce + depForce.GetSafeNormal() * 0.125;
		}
	}

	if (displacement.IsZero())
		return;

	if (OnLocationSet == nullptr)
		return;

	//Try to go to that location
	FHitResult hit = FHitResult(NoInit);
	if (world->SweepSingleByChannel(hit, location, location + displacement, rotation, channel, shape, comQueryParams))
	{
		UModularMoverComponent* mover = hit.GetActor()->GetComponentByClass<UModularMoverComponent>();
		if (mover && mover->UpdatedPrimitive == hit.Component)
		{
			OnLocationSet(location + displacement);
			maxDepth--;
			if (maxDepth >= 0)
				mover->OverlapSolver(maxDepth);
		}
		else
		{
			if (hit.Component->IsSimulatingPhysics())
				OnLocationSet(hit.Location + displacement.GetClampedToMaxSize(0.125));
			else
				OnLocationSet(hit.Location);
		}
	}
	else
	{
		OnLocationSet(location + displacement);
	}
}


#pragma endregion

#pragma region Movement Mode


void UModularMoverComponent::AddContingentMoveMode(TSubclassOf<UBaseContingentMove> Class)
{
	if (!_subSystem)
		return;
	FContingentMoveInfos NewMoveInfos;
	if (_subSystem->AddContingentLibrary(NewMoveInfos, Class))
	{
		int index = -1;
		if (ContingentMoveState.Num() > 0)
		{
			index = ContingentMoveState.IndexOfByPredicate([NewMoveInfos](const FContingentMoveInfos& info)-> bool
			{
				return info.BaseInfos.ModeName == NewMoveInfos.BaseInfos.ModeName;
			});
		}
		if (!ContingentMoveState.IsValidIndex(index))
		{
			ContingentMoveState.Add(NewMoveInfos);
		}
	}
}

void UModularMoverComponent::RemoveContingentMoveMode(FName MoveName)
{
	if (!_subSystem)
		return;
	if (_subSystem->RemoveContingentLibrary(MoveName))
	{
		if (ContingentMoveState.Num() <= 0)
			return;
		const int index = ContingentMoveState.IndexOfByPredicate([MoveName](const FContingentMoveInfos& infos)-> bool
		{
			return infos.BaseInfos.ModeName == MoveName;
		});
		if (ContingentMoveState.IsValidIndex(index))
		{
			ContingentMoveState.RemoveAt(index);
		}
	}
}

void UModularMoverComponent::AddTransientMoveMode(TSubclassOf<UBaseTransientMove> Class)
{
	if (!_subSystem)
		return;
	FTransientMoveInfos NewMoveInfos;
	if (_subSystem->AddTransientLibrary(NewMoveInfos, Class))
	{
		int index = -1;
		if (TransientMoveState.Num() > 0)
		{
			index = TransientMoveState.IndexOfByPredicate([NewMoveInfos](const FTransientMoveInfos& info)-> bool
			{
				return info.BaseInfos.ModeName == NewMoveInfos.BaseInfos.ModeName;
			});
		}
		if (!TransientMoveState.IsValidIndex(index))
		{
			TransientMoveState.Add(NewMoveInfos);
		}
	}
}

void UModularMoverComponent::RemoveTransientMoveMode(FName MoveName)
{
	if (!_subSystem)
		return;
	if (_subSystem->RemoveTransientLibrary(MoveName))
	{
		if (TransientMoveState.Num() <= 0)
			return;
		const int index = TransientMoveState.IndexOfByPredicate([MoveName](const FTransientMoveInfos& infos)-> bool
		{
			return infos.BaseInfos.ModeName == MoveName;
		});
		if (TransientMoveState.IsValidIndex(index))
		{
			TransientMoveState.RemoveAt(index);
		}
	}
}


#pragma endregion


#pragma region Flow Mode

// Stand alone mode

#pragma endregion


#pragma region Movement


FVector UModularMoverComponent::ComputeLinearVelocity(const FLinearMechanic AttemptedMovement, const FVector currentLinearVelocity, const float deltaTime, bool UseReduction)
{
	const float speed = AttemptedMovement.Acceleration.SquaredLength() > 0 ? AttemptedMovement.Acceleration.Length() : AttemptedMovement.DecelerationSpeed;
	FVector velocity = FMath::VInterpConstantTo(currentLinearVelocity, AttemptedMovement.Acceleration.GetSafeNormal() * AttemptedMovement.TerminalVelocity, deltaTime, speed);

	if (UseReduction)
		velocity -= currentLinearVelocity;

	return velocity;
}

FVector UModularMoverComponent::ComputeAngularVelocity(FAngularMechanic AttemptedMovement, FVector CurrentAngularVelocity, const FQuat CurrentOrientation, FVector Gravity,
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

	FVector result = GetAngularVelocity(CurrentOrientation, angularPart, Gravity, OffsetRotation) / deltaTime;

	if (UseReduction)
		result -= CurrentAngularVelocity;

	return result;
}


void UModularMoverComponent::MoveBody(FBodyInstance* Body, const FMechanicProperties movement, const float Delta) const
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
		linearMovement = ComputeLinearVelocity(movement.Linear, currentLinearVelocity, Delta, true);

		if (DebugMode == EDebugMode::LinearMovement)
		{
			const float moveDirection = FMath::Clamp(currentLinearVelocity.GetSafeNormal() | movement.Linear.Acceleration.GetSafeNormal(), -1, 1);
			UKismetSystemLibrary::PrintString(this, FString::Printf(
				                                  TEXT("[LinearVelocity] - Current Vel (%f); Move Dir (%f); Movement Vel (%f)"),
				                                  currentLinearVelocity.Length(), moveDirection, linearMovement.Length()), true,
			                                  false, FColor::Magenta, 60, "ComputeLinearVelocity");
		}
	}


	//Angular Part
	{
		angularMovement = ComputeAngularVelocity(movement.Angular, currentAngularVelocity, initialTransform.GetRotation(), movement.Gravity, Delta, true, RotationOffset);

		if (DebugMode == EDebugMode::AngularMovement)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(
				                                  TEXT("[AngularVelocity] - Current Vel (%f); Rotation Vel (%f)"),
				                                  FMath::RadiansToDegrees(currentAngularVelocity.Length()), FMath::RadiansToDegrees(angularMovement.Length())), true,
			                                  false, FColor::Magenta, 60, "ComputeAngularVelocity");
		}
	}

	//Wake up and apply movement
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
                                                        const FRotator OffsetRotation)
{
	TArray<FMomentum> result;
	result.Add(currentMomentum);
	for (int i = 0; i < SampleCount; i++)
	{
		FMomentum lastItem = result[result.Num() - 1];
		FMomentum iteration;
		iteration.Mass = currentMomentum.Mass;

		//Linear part
		{
			iteration.LinearVelocity = ComputeLinearVelocity(inputMovement.Linear, lastItem.LinearVelocity, timeStep);
			iteration.Transform.SetLocation(lastItem.Transform.GetLocation() + iteration.LinearVelocity * timeStep);
		}

		//Angular part
		{
			iteration.AngularVelocity = ComputeAngularVelocity(inputMovement.Angular, lastItem.AngularVelocity, lastItem.Transform.GetRotation(), inputMovement.Gravity, timeStep, false,
			                                                   OffsetRotation);
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
