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
	{
		UpdatedPrimitive->SetCollisionEnabled(IsIgnoringCollision() ? ECollisionEnabled::QueryAndProbe : ECollisionEnabled::QueryAndPhysics);
	}
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
			//Update surfaces
			if (ActiveContingentMove.IsValid())
			{
				for (int i = 0; i < ActiveContingentMove.Surfaces.Num(); i++)
				{
					ActiveContingentMove.Surfaces[i].UpdateTracking(DeltaTime);
				}
			}

			//Building Momentum
			_currentMomentum = FMomentum();
			_currentMomentum.Transform = UPhysicToolbox::GetRigidBodyTransform(BodyInstance);
			_currentMomentum.LinearVelocity = UPhysicToolbox::GetRigidBodyLinearVelocity(BodyInstance);
			_currentMomentum.AngularVelocity = UPhysicToolbox::GetRigidBodyAngularVelocity(BodyInstance);
			_currentMomentum.Mass = GetMass();
			_currentMomentum.Gravity = _lastGravity;
			_currentMomentum.SetShape(UpdatedPrimitive->GetCollisionShape());
			_currentMomentum.Surfaces = ActiveContingentMove.Surfaces;

			// Check modifications
			EvaluateMovementDiff(BodyInstance, _currentMomentum, _inputPool);
			if (DebugMode == EDebugMode::SurfaceDetection)
			{
				for (const auto surface : _currentMomentum.Surfaces)
				{
					UDebugToolbox::DrawDebugCircleOnHit(surface.HitResult, 20, FColor::Green);
					UKismetSystemLibrary::DrawDebugArrow(this, surface.HitResult.ImpactPoint,
					                                     surface.HitResult.ImpactPoint + surface.GetVelocityAt(_currentMomentum.Transform.GetLocation(), DeltaTime), 20, FColor::Green);
				}
			}

			// Process and blend
			FMechanicProperties move = ProcessContingentMoves(_currentMomentum, DeltaTime);
			move = ProcessTransientMoves(move, _currentMomentum, DeltaTime);

			//Affect variables
			_bMoveDisableCollision = move.IgnoreCollision;
			_lastGravity = move.Gravity;

			//Check hit on snap
			if (move.Linear.SnapDisplacement.SquaredLength() > 0)
			{
				FHitResult snapHit;
				FCollisionQueryParams queryParams;
				queryParams.AddIgnoredActor(GetOwner());
				queryParams.AddIgnoredComponents(IgnoredCollisionComponents);
				if (GetWorld()->SweepSingleByChannel(snapHit, _currentMomentum.Transform.GetLocation(), _currentMomentum.Transform.GetLocation() + move.Linear.SnapDisplacement
				                                     , _currentMomentum.Transform.GetRotation(), UpdatedPrimitive->GetCollisionObjectType(), UpdatedPrimitive->GetCollisionShape(-1),
				                                     queryParams))
				{
					move.Linear.SnapDisplacement = snapHit.Location - _currentMomentum.Transform.GetLocation();
				}
			}

			//Evaluate surface movement transform
			const FVector surfaceLinear = UStructExtension::GetAverageSurfaceVelocityAt(_currentMomentum.Surfaces, _currentMomentum.Transform.GetLocation(), DeltaTime, ECR_MAX);
			const FVector surfaceAngular = UStructExtension::GetAverageSurfaceAngularSpeed(_currentMomentum.Surfaces, ECR_MAX);
			const FQuat surfaceAngularQuat = FQuat(surfaceAngular.GetSafeNormal(), FMath::DegreesToRadians(surfaceAngular.Length()) * DeltaTime);
			const FTransform surfaceVelocities = FTransform(surfaceAngularQuat, surfaceLinear, FVector::OneVector);

			//Move
			MoveBody(BodyInstance, surfaceVelocities, move, DeltaTime);
		}
	}
	// ...
	Super::AsyncPhysicsTickComponent(DeltaTime, SimTime);
}

void UModularMoverComponent::EvaluateMovementOnSurfaces(const FMoverCheckRequest Request, TArray<FExpandedHitResult> SurfacesHits)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("EvaluateMovementOnSurfaces");

	CheckContingentMoves(Request, SurfacesHits);
	CheckTransientMoves(Request, SurfacesHits);

	if (DebugMode == EDebugMode::SurfaceDetection)
	{
		for (int i = 0; i < SurfacesHits.Num(); i++)
		{
			if (SurfacesHits[i].HitResult.GetComponent())
			{
				UDebugToolbox::DrawDebugCircleOnHit(SurfacesHits[i].HitResult, 10, FColor::White, 0, 0.5);
			}
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

		if (UpdatedPrimitive)
		{
			OnMoveCheck(this, FMoverCheckRequest(UpdatedPrimitive->GetBodyInstance(), _currentMomentum, _inputPool));
		}
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

		if (UpdatedPrimitive)
		{
			OnMoveCheck(this, FMoverCheckRequest(UpdatedPrimitive->GetBodyInstance(), _currentMomentum, _inputPool));
		}
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

		if (UpdatedPrimitive)
		{
			OnMoveCheck(this, FMoverCheckRequest(UpdatedPrimitive->GetBodyInstance(), _currentMomentum, _inputPool));
		}
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

		if (UpdatedPrimitive)
		{
			OnMoveCheck(this, FMoverCheckRequest(UpdatedPrimitive->GetBodyInstance(), _currentMomentum, _inputPool));
		}
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


bool UModularMoverComponent::IsIgnoringCollision() const
{
	bool ignore = bDisableCollision;
	if (!ignore)
	{
		ignore = _bMoveDisableCollision;
	}
	return ignore;
}


FVector UModularMoverComponent::GetCurrentScanSurfaceVector() const
{
	return ActiveTransientMove.IsValid() ? ActiveTransientMove.ScanSurfaceVector : ActiveContingentMove.ScanSurfaceVector;
}


float UModularMoverComponent::GetCurrentScanSurfaceOffset() const
{
	return ActiveTransientMove.IsValid() ? ActiveTransientMove.ScanSurfaceOffset : ActiveContingentMove.ScanSurfaceOffset;
}


void UModularMoverComponent::EvaluateMovementDiff(FBodyInstance* Body, const FMomentum Momentum, const FMoverInputPool InputPool)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("EvaluateMovementDiff");
	const FVector _curLocation = Momentum.Transform.GetLocation();
	const FQuat _curRotation = Momentum.Transform.GetRotation();
	_lastLocation = _curLocation;
	_lastRotation = _curRotation;
	const FQuat diff = (_curRotation.Inverse() * _lastRotation_trigger);
	FVector axis;
	float angle;
	diff.ToAxisAndAngle(axis, angle);
	if ((_curLocation - _lastLocation_trigger).Length() > MoveTriggerTolerance || FMath::RadiansToDegrees(angle) > RotateTriggerTolerance || TrackSurfaceMovementUpdate(Momentum.Surfaces))
	{
		OnComponentMoved.Broadcast(this, FMoverCheckRequest(Body, Momentum, InputPool));
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


bool UModularMoverComponent::TrackSurfaceMovementUpdate(const TArray<FSurface>& Surfaces)
{
	bool mustUpdate = false;
	//Check
	for (const auto surface : Surfaces)
	{
		if (!surface.HitResult.Component.IsValid())
			continue;
		if (!_lastSurfaceTransform.Contains(surface.HitResult.GetComponent()))
		{
			mustUpdate = true;
			break;
		}
		const FVector cLoc = surface.HitResult.Component->GetComponentLocation();
		const FQuat cRot = surface.HitResult.Component->GetComponentRotation().Quaternion();
		const FVector lLoc = _lastSurfaceTransform[surface.HitResult.GetComponent()].GetLocation();
		const FQuat lRot = _lastSurfaceTransform[surface.HitResult.GetComponent()].GetRotation();

		const FQuat diff = (cRot.Inverse() * lRot);
		FVector axis;
		float angle;
		diff.ToAxisAndAngle(axis, angle);

		if ((cLoc - lLoc).Length() > MoveTriggerTolerance
			|| FMath::RadiansToDegrees(angle) > RotateTriggerTolerance)
		{
			mustUpdate = true;
			break;
		}
	}

	if (!mustUpdate)
		return false;

	//Check rotations
	_lastSurfaceTransform.Empty();
	for (const auto surface : Surfaces)
	{
		if (!surface.HitResult.Component.IsValid())
			continue;
		const FVector location = surface.HitResult.Component->GetComponentLocation();
		const FQuat rotation = surface.HitResult.Component->GetComponentRotation().Quaternion();
		_lastSurfaceTransform.Add(surface.HitResult.GetComponent(), FTransform(rotation, location, FVector(1)));
	}
	return true;
}


void UModularMoverComponent::OnMoveCheck(UModularMoverComponent* Mover, FMoverCheckRequest Request)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("OnMoveCheck");
	if (Mover != this)
		return;
	if (bUseAsyncSurfaceDetection)
	{
		const auto request = AsyncDetectOverlapHits(Request.Momentum.Transform, &_onMainSurfaceChk, GetCurrentScanSurfaceVector(), GetCurrentScanSurfaceOffset());
		if (_chkRequestMap.Contains(request))
			_chkRequestMap[request] = Request;
		else
			_chkRequestMap.Add(request, Request);
		return;
	}

	TArray<FExpandedHitResult> surfaceHits;
	DetectOverlapHits(Request.Momentum.Transform, surfaceHits, GetCurrentScanSurfaceVector(), GetCurrentScanSurfaceOffset());
	int maxDepth = 0;
	FixOverlapHits(maxDepth, Request.Momentum.Transform, surfaceHits, [&](FVector location)-> void
	{
		Request.Momentum.Transform.SetLocation(location);
	});
	EvaluateMovementOnSurfaces(Request, surfaceHits);
}


void UModularMoverComponent::OnMainSurfaceCheckDone(const FTraceHandle& TraceHandle, FTraceDatum& TraceData)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("OnMainSurfaceCheckDone");
	if (!_chkRequestMap.Contains(TraceHandle))
		return;
	FMoverCheckRequest request = _chkRequestMap[TraceHandle];
	_chkRequestMap.Remove(TraceHandle);
	TArray<FExpandedHitResult> outHitResults;
	UPhysicToolbox::PostPhysicTrace_internal(TraceData.OutHits, outHitResults, TraceData.TraceChannel, TraceData.CollisionParams.CollisionQueryParam, ESurfaceTraceHitType::MAX,
	                                         MinDepthSurface);
	int maxDepth = 0;
	FixOverlapHits(maxDepth, request.Momentum.Transform, outHitResults, [&](FVector location)-> void
	{
		request.Momentum.Transform.SetLocation(location);
	});
	EvaluateMovementOnSurfaces(request, outHitResults);
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
	FVector offset = scanVector.GetSafeNormal() * CounterDirectionOffset;
	if (DebugMode == EDebugMode::SurfaceDetection)
	{
		FHitResult hit;
		hit.Normal = scanVector.GetSafeNormal();
		hit.ImpactNormal = scanVector.GetSafeNormal();
		hit.ImpactPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, Transform, -scanVector) - offset;
		hit.Component = UpdatedPrimitive;
		UDebugToolbox::DrawDebugCircleOnHit(hit, 40, FColor::Cyan, 0, 0.5);
		hit.ImpactPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, Transform, scanVector) + scanVector + offset;
		UDebugToolbox::DrawDebugCircleOnHit(hit, 40, FColor::Cyan, 0, 0.5);
	}
	FCollisionQueryParams query = FCollisionQueryParams::DefaultQueryParam;
	query.OwnerTag = "Mover";
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
	FVector offset = scanVector.GetSafeNormal() * CounterDirectionOffset;
	if (DebugMode == EDebugMode::SurfaceDetection)
	{
		FHitResult hit;
		hit.Normal = scanVector.GetSafeNormal();
		hit.ImpactNormal = scanVector.GetSafeNormal();
		hit.ImpactPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, Transform, -scanVector) - offset;
		hit.Component = UpdatedPrimitive;
		UDebugToolbox::DrawDebugCircleOnHit(hit, 40, FColor::Cyan, 0, 0.5);
		hit.ImpactPoint = UPhysicToolbox::GetPointOnShapeInDirection(shape, Transform, scanVector) + scanVector + offset;
		UDebugToolbox::DrawDebugCircleOnHit(hit, 40, FColor::Cyan, 0, 0.5);
	}
	FCollisionQueryParams query = FCollisionQueryParams::DefaultQueryParam;
	query.OwnerTag = "Mover";
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

void UModularMoverComponent::CheckContingentMoves(const FMoverCheckRequest Request, const TArray<FExpandedHitResult> SurfacesHits)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("CheckContingentMoves");
	if (!_subSystem)
		return;
	_chkContingentQueue.Empty();
	for (int i = 0; i < ContingentMoveState.Num(); i++)
	{
		//check
		if (const auto MovementMode = _subSystem->GetContingentMoveObject(ContingentMoveState[i].BaseInfos.ModeName))
		{
			int surfacesIndexesFlag = 0;
			if (MovementMode->CheckContingentMovement(this, SurfacesHits, ContingentMoveState[i], Request.Momentum, _movementInput, Request.InputPool, ContingentMoveState, TransientMoveState,
			                                          CustomProperties, surfacesIndexesFlag))
			{
				//enqueue
				_chkContingentQueue.Enqueue(FVector(i, ContingentMoveState[i].BaseInfos.Priority, surfacesIndexesFlag));
			}
		}
	}

	int masterPriority = -1;
	int masterIndex = -1;
	int masterSurfaceFlag = 0;
	FVector item;
	while (_chkContingentQueue.Dequeue(item))
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

	if (!ContingentMoveState.IsValidIndex(masterIndex))
		return;

	//check changes in active
	if (ActiveContingentMove.IsValid() && ActiveContingentMove.ModeName == ContingentMoveState[masterIndex].BaseInfos.ModeName)
	{
		auto surfaceActives = UConversionToolbox::FlagToBoolArray(masterSurfaceFlag);
		TArray<FSurface> updatedSurfaces;
		for (int i = 0; i < surfaceActives.Num(); i++)
		{
			if (!SurfacesHits.IsValidIndex(i))
				continue;
			if (!surfaceActives[i])
				continue;
			const int surfaceIndex = ActiveContingentMove.Surfaces.IndexOfByPredicate([SurfacesHits, i](const FSurface& s)-> bool
			{
				return s.HitResult.Component == SurfacesHits[i].HitResult.Component;
			});
			if (!ActiveContingentMove.Surfaces.IsValidIndex(surfaceIndex))
			{
				updatedSurfaces.Add(FSurface(Request.MoverBody, SurfacesHits[i].HitResult, SurfacesHits[i].OffsetType));
				continue;
			}
			ActiveContingentMove.Surfaces[surfaceIndex].UpdateHit(Request.MoverBody, SurfacesHits[i].HitResult, SurfacesHits[i].OffsetType);
			updatedSurfaces.Add(ActiveContingentMove.Surfaces[surfaceIndex]);
		}
		ActiveContingentMove.Surfaces = updatedSurfaces;
		return;
	}
	// Set new active
	if (const auto MovementMode = _subSystem->GetContingentMoveObject(ContingentMoveState[masterIndex].BaseInfos.ModeName))
	{
		FMoverModeSelection selection = FMoverModeSelection();
		auto surfaceActives = UConversionToolbox::FlagToBoolArray(masterSurfaceFlag);
		for (int i = 0; i < surfaceActives.Num(); i++)
		{
			if (!SurfacesHits.IsValidIndex(i))
				continue;
			if (!surfaceActives[i])
				continue;
			selection.Surfaces.Add(FSurface(Request.MoverBody, SurfacesHits[i].HitResult, SurfacesHits[i].OffsetType));
		}
		selection.ScanSurfaceOffset = MovementMode->ScanSurfaceOffset;
		selection.ScanSurfaceVector = MovementMode->ScanSurfaceVector;
		selection.ModeName = ContingentMoveState[masterIndex].BaseInfos.ModeName;

		ActiveContingentMove = selection;
	}
}

FMechanicProperties UModularMoverComponent::ProcessContingentMoves(const FMomentum currentMomentum, const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ProcessContingentMoves");
	FMechanicProperties move = FMechanicProperties();
	FString inactiveDebug;
	float activeWeight = 0;
	int activeIndex = -1;

	//Phase 1
	for (int i = 0; i < ContingentMoveState.Num(); i++)
	{
		if (ActiveContingentMove.IsValid() && ContingentMoveState[i].BaseInfos.ModeName == ActiveContingentMove.ModeName)
		{
			ActiveContingentMove.bConsumedActivation = true;

			//Update blends
			ContingentMoveState[i].BaseInfos.CurrentWeight = FMath::FInterpConstantTo(ContingentMoveState[i].BaseInfos.CurrentWeight, 1, DeltaTime,
			                                                                          ContingentMoveState[i].BaseInfos.BlendSpeed.X);
			activeWeight = ContingentMoveState[i].BaseInfos.CurrentWeight;
			activeIndex = i;
			// Update infos status
			ContingentMoveState[i].BaseInfos.bIsActiveMode = true;
			ContingentMoveState[i].BaseInfos.TimeOnMode += DeltaTime;
			ContingentMoveState[i].TotalTimeOnMode += DeltaTime;
		}
		else
		{
			//Update blends
			ContingentMoveState[i].BaseInfos.CurrentWeight = FMath::FInterpConstantTo(ContingentMoveState[i].BaseInfos.CurrentWeight, 0, DeltaTime,
			                                                                          ContingentMoveState[i].BaseInfos.BlendSpeed.Y);
			// Update infos status
			ContingentMoveState[i].BaseInfos.bIsActiveMode = false;
			ContingentMoveState[i].BaseInfos.TimeOnMode = 0;
		}
	}

	if (_subSystem)
	{
		//Phase 2
		if (ContingentMoveState.IsValidIndex(activeIndex))
		{
			if (const auto MovementMode = _subSystem->GetContingentMoveObject(ContingentMoveState[activeIndex].BaseInfos.ModeName))
			{
				// Process
				const auto name = ContingentMoveState[activeIndex].BaseInfos.ModeName;
				move = MovementMode->ProcessContingentMovement(this, ContingentMoveState[activeIndex], currentMomentum, _movementInput, _inputPool, DeltaTime);
				ContingentMoveState[activeIndex].BaseInfos.ModeName = name;
				// Blend values
				const FVector snap = move.Linear.SnapDisplacement;
				move.Scale(activeWeight);
				move.Linear.SnapDisplacement = snap;
			}
		}

		//Phase 3
		float blendLeft = 1 - activeWeight;
		inactiveDebug = FString();
		if (blendLeft > 0)
		{
			for (int i = 0; i < ContingentMoveState.Num(); i++)
			{
				if (blendLeft <= 0)
					break;
				if (i == activeIndex)
					continue;
				if (ContingentMoveState[i].BaseInfos.CurrentWeight <= 0)
					continue;

				if (const auto MovementMode = _subSystem->GetContingentMoveObject(ContingentMoveState[i].BaseInfos.ModeName))
				{
					// Process
					const auto name = ContingentMoveState[i].BaseInfos.ModeName;
					auto auxMove = MovementMode->ProcessContingentMovement(this, ContingentMoveState[i], currentMomentum, _movementInput, _inputPool, DeltaTime);
					ContingentMoveState[i].BaseInfos.ModeName = name;
					// Blend values
					const float blendValue = FMath::Min(blendLeft, ContingentMoveState[i].BaseInfos.CurrentWeight);
					auxMove.Scale(blendValue);
					bool ignoreCol = move.IgnoreCollision;
					const FVector snap = move.Linear.SnapDisplacement;
					move = move + auxMove;
					move.IgnoreCollision = ignoreCol;
					move.Linear.SnapDisplacement = snap;
					blendLeft -= blendValue;

					//Debug
					inactiveDebug.Append(FString::Printf(
						TEXT("[Inactive] - [Index (%d); Weight (%f); Name (%s)]\n"), i, ContingentMoveState[i].BaseInfos.CurrentWeight,
						*ContingentMoveState[i].BaseInfos.ModeName.ToString()));
				}
			}
		}
	}

	if (DebugMode == EDebugMode::MovementProcessing)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
			                                  TEXT("{Contingent Moves}-------------------------------- \n[Active] - [Index (%d); Weight (%f); Name (%s)]\n%s-------------------------"),
			                                  activeIndex, activeWeight, *ActiveContingentMove.ModeName.ToString(), *inactiveDebug), true,
		                                  false, FColor::Magenta, 60, "ContingentMoveInfos");
	}

	return move;
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

void UModularMoverComponent::CheckTransientMoves(const FMoverCheckRequest Request, const TArray<FExpandedHitResult> Surfaces)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("CheckTransientMoves");
	if (!_subSystem)
		return;
	_chkTransientQueue.Empty();
	for (int i = 0; i < TransientMoveState.Num(); i++)
	{
		//check
		if (const auto MovementMode = _subSystem->GetTransientMoveObject(TransientMoveState[i].BaseInfos.ModeName))
		{
			if (MovementMode->CheckTransientMovement(this, Surfaces, TransientMoveState[i], Request.Momentum, _movementInput, Request.InputPool, ContingentMoveState, TransientMoveState,
			                                         CustomProperties))
			{
				//enqueue
				_chkTransientQueue.Enqueue(FVector(i, TransientMoveState[i].BaseInfos.Priority, 0));
			}
		}
	}

	int masterPriority = -1;
	int masterIndex = -1;
	FVector item;
	while (_chkTransientQueue.Dequeue(item))
	{
		//check priority
		if (item.Y < masterPriority)
			continue;
		//set index
		masterIndex = item.X;
		//set high priority
		masterPriority = item.Y;
	}

	if (!TransientMoveState.IsValidIndex(masterIndex))
		return;

	// Set new active
	if (const auto MovementMode = _subSystem->GetTransientMoveObject(TransientMoveState[masterIndex].BaseInfos.ModeName))
	{
		FMoverModeSelection selection = FMoverModeSelection();
		selection.ModeName = TransientMoveState[masterIndex].BaseInfos.ModeName;

		ActiveTransientMove = selection;
	}
}

FMechanicProperties UModularMoverComponent::ProcessTransientMoves(const FMechanicProperties ContingentMoveResult, const FMomentum currentMomentum, const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ProcessTransientMoves");
	FMechanicProperties move = ContingentMoveResult;
	FString activeDebug;
	FString inactiveDebug;
	float activeBlendSpeed = 0;
	float activeWeight = 0;
	int activeIndex = ActiveTransientMove.IsValid()
		                  ? TransientMoveState.IndexOfByPredicate([&](const FTransientMoveInfos& infos)-> bool { return infos.BaseInfos.ModeName == ActiveTransientMove.ModeName; })
		                  : -1;

	//Phase 1
	if (TransientMoveState.IsValidIndex(activeIndex))
	{
		if (!ActiveTransientMove.bConsumedActivation)
		{
			//Restarted
			TransientMoveState[activeIndex].BaseInfos.bIsActiveMode = false;
			TransientMoveState[activeIndex].BaseInfos.TimeOnMode = 0;
			TransientMoveState[activeIndex].UpdatePhase();
		}
		ActiveTransientMove.bConsumedActivation = true;

		//Update blends
		if (!TransientMoveState[activeIndex].BaseInfos.bIsActiveMode)
			TransientMoveState[activeIndex].BaseInfos.CurrentWeight = 0;
		activeBlendSpeed = TransientMoveState[activeIndex].BaseInfos.BlendSpeed.X;
		TransientMoveState[activeIndex].BaseInfos.CurrentWeight = FMath::FInterpConstantTo(TransientMoveState[activeIndex].BaseInfos.CurrentWeight, 1, DeltaTime, activeBlendSpeed);
		activeWeight = TransientMoveState[activeIndex].BaseInfos.CurrentWeight;

		// Update infos status
		TransientMoveState[activeIndex].BaseInfos.bIsActiveMode = true;
		TransientMoveState[activeIndex].BaseInfos.TimeOnMode += DeltaTime;
		TransientMoveState[activeIndex].UpdatePhase();

		if (_subSystem)
		{
			if (const auto MovementMode = _subSystem->GetTransientMoveObject(TransientMoveState[activeIndex].BaseInfos.ModeName))
			{
				// Process
				const auto name = TransientMoveState[activeIndex].BaseInfos.ModeName;
				move = MovementMode->ProcessTransientMovement(this, ContingentMoveResult, TransientMoveState[activeIndex], currentMomentum, _movementInput, _inputPool, DeltaTime);
				TransientMoveState[activeIndex].BaseInfos.ModeName = name;
				// Blend values
				const FVector snap = move.Linear.SnapDisplacement;
				move.Scale(activeWeight);
				move.Linear.SnapDisplacement = snap;
			}
		}

		//Try to disable
		const float outDuration = 1 / TransientMoveState[activeIndex].BaseInfos.BlendSpeed.Y;
		if (TransientMoveState[activeIndex].BaseInfos.TimeOnMode >= FMath::Clamp(TransientMoveState[activeIndex].TotalDuration() - outDuration, 0, TNumericLimits<float>::Max()))
			ActiveTransientMove = FMoverModeSelection();

		//Debug
		activeDebug = FString::Printf(TEXT("Time (%f), Phase (%s), Lenght (%f), outDuration (%f)"), TransientMoveState[activeIndex].BaseInfos.TimeOnMode,
		                              *TransientMoveState[activeIndex].CurrentPhase.PhaseName.ToString(), TransientMoveState[activeIndex].TotalDuration(), outDuration);
	}

	//Phase 2
	float blendLeft = 1 - activeWeight;
	inactiveDebug = FString();
	for (int i = 0; i < TransientMoveState.Num(); i++)
	{
		if (i == activeIndex)
			continue;

		//Update blends
		const float speed = activeBlendSpeed > 0 ? activeBlendSpeed : TransientMoveState[i].BaseInfos.BlendSpeed.Y;
		TransientMoveState[i].BaseInfos.CurrentWeight = FMath::FInterpConstantTo(TransientMoveState[i].BaseInfos.CurrentWeight, 0, DeltaTime, speed);
		// Update infos status
		TransientMoveState[i].BaseInfos.bIsActiveMode = false;
		TransientMoveState[i].BaseInfos.TimeOnMode = 0;

		if (blendLeft <= 0)
			continue;
		if (TransientMoveState[i].BaseInfos.CurrentWeight <= 0)
			continue;

		if (_subSystem)
		{
			if (const auto MovementMode = _subSystem->GetTransientMoveObject(TransientMoveState[i].BaseInfos.ModeName))
			{
				// Process
				const auto name = TransientMoveState[i].BaseInfos.ModeName;
				auto auxMove = MovementMode->ProcessTransientMovement(this, ContingentMoveResult, TransientMoveState[i], currentMomentum, _movementInput, _inputPool, DeltaTime);
				TransientMoveState[i].BaseInfos.ModeName = name;
				// Blend values
				const float blendValue = FMath::Min(blendLeft, TransientMoveState[i].BaseInfos.CurrentWeight);
				auxMove.Scale(blendValue);
				bool ignoreCol = move.IgnoreCollision;
				const FVector snap = move.Linear.SnapDisplacement;
				move = move + auxMove;
				move.IgnoreCollision = ignoreCol;
				move.Linear.SnapDisplacement = snap;
				blendLeft -= blendValue;

				//Debug
				inactiveDebug.Append(FString::Printf(
					TEXT("[Inactive] - [Index (%d); Weight (%f); Name (%s), Phase (%s), Lenght (%f)]\n"), i, TransientMoveState[i].BaseInfos.CurrentWeight,
					*TransientMoveState[i].BaseInfos.ModeName.ToString(),
					*TransientMoveState[i].CurrentPhase.PhaseName.ToString(), TransientMoveState[i].TotalDuration()));
			}
		}
	}

	if (DebugMode == EDebugMode::MovementProcessing)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
			                                  TEXT("{Transient Moves}-------------------------------- \n[Active] - [Index (%d); Weight (%f); Name (%s) %s]\n%s-------------------------"),
			                                  activeIndex, activeWeight, *ActiveTransientMove.ModeName.ToString(), *activeDebug, *inactiveDebug), true,
		                                  false, FColor::Magenta, 60, "TransientMoveInfos");
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
	const FVector relativeCurrentVelocity = currentLinearVelocity - SurfacesLinearVelocity;
	const float speed = AttemptedMovement.Acceleration.SquaredLength() > 0 ? AttemptedMovement.Acceleration.Length() : AttemptedMovement.DecelerationSpeed;
	const FVector relativeVelocity = FMath::VInterpConstantTo(relativeCurrentVelocity,AttemptedMovement.Acceleration.GetSafeNormal() * AttemptedMovement.TerminalVelocity, deltaTime, speed);
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


void UModularMoverComponent::MoveBody(FBodyInstance* Body, const FTransform SurfaceMovement, const FMechanicProperties movement, const float Delta) const
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
		linearMovement = ComputeLinearVelocity(movement.Linear, currentLinearVelocity, SurfaceMovement.GetLocation(), Delta, true);

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
		angularMovement = ComputeAngularVelocity(movement.Angular, currentAngularVelocity, initialTransform.GetRotation(), SurfaceMovement.GetRotation(), movement.Gravity, Delta, true,
		                                         RotationOffset);

		if (DebugMode == EDebugMode::AngularMovement)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(
				                                  TEXT("[AngularVelocity] - Current Vel (%f); Rotation Vel (%f)"),
				                                  FMath::RadiansToDegrees(currentAngularVelocity.Length()), FMath::RadiansToDegrees(angularMovement.Length())), true,
			                                  false, FColor::Magenta, 60, "ComputeAngularVelocity");
		}
	}

	//Wake up and apply movement
	UPhysicToolbox::RigidBodySetWorldLocation(Body, initialTransform.GetLocation() + movement.Linear.SnapDisplacement);
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
	const FVector surfaceLinear = UStructExtension::GetAverageSurfaceVelocityAt(currentMomentum.Surfaces, currentMomentum.Transform.GetLocation(), timeStep, ECR_MAX);
	const FVector surfaceAngular = UStructExtension::GetAverageSurfaceAngularSpeed(currentMomentum.Surfaces, ECR_MAX);
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
