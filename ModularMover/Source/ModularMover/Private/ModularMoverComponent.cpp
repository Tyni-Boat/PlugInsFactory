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
	SetTickGroup(TG_PrePhysics);
	SetAsyncPhysicsTickEnabled(true);
	InitialRegistration();
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
}


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
			const FTransform bodyTransform = BodyInstance->GetUnrealWorldTransform();
			BodyInstance->SetCollisionEnabled(IsIgnoringCollision() ? ECollisionEnabled::QueryAndProbe : ECollisionEnabled::QueryAndPhysics);
			EvaluateMovementDiff(BodyInstance);

			FMechanicProperties move = DebugMovement;
			//BodyMomentum.Transform = bodyTransform;
			MoveBody(BodyInstance, move, DeltaTime);
		}
	}
	// ...
	Super::AsyncPhysicsTickComponent(DeltaTime, SimTime);
}

void UModularMoverComponent::EvaluateMovementSurface(const FTransform Transform, TArray<FExpandedHitResult> Surfaces)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("EvaluateMovementSurface");
	if (DebugMode == EDebugMode::SurfaceDetection)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Transform.GetLocation(), 25, 8);
		UKismetSystemLibrary::DrawDebugArrow(this, Transform.GetLocation(), Transform.GetLocation() + Transform.GetRotation().Vector() * 100, 100, FColor::White, 0, 3);
		for (int i = 0; i < Surfaces.Num(); i++)
		{
			if (Surfaces[i].HitResult.GetComponent())
				UDebugToolbox::DrawDebugCircleOnHit(Surfaces[i].HitResult, 40, FColor::White, Surfaces[i].HitResult.GetComponent()->GetWorld()->DeltaTimeSeconds, 0.5);
		}
	}
}


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


void UModularMoverComponent::EvaluateMovementDiff(const FBodyInstance* body)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("EvaluateMovementDiff");
	if (!body)
		return;
	const FTransform curTr = body->GetUnrealWorldTransform_AssumesLocked();
	const FVector _curLocation = curTr.GetLocation();
	const FQuat _curRotation = curTr.GetRotation();
	_lastLocation = _curLocation;
	_lastRotation = _curRotation;
	const FQuat diff = (_curRotation.Inverse() * _lastRotation_trigger);
	FVector axis;
	float angle;
	diff.ToAxisAndAngle(axis, angle);
	if ((_curLocation - _lastLocation_trigger).Length() > MoveTriggerTolerance
		|| FMath::RadiansToDegrees(angle) > RotateTriggerTolerance)
	{
		OnComponentMoved.Broadcast(this, curTr);
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
	EvaluateMovementSurface(Transform, surfaceHits);
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
	EvaluateMovementSurface(transform, outHitResults);
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

#pragma region Flow Mode

// Stand alone mode

#pragma endregion

#pragma region Movement


FVector UModularMoverComponent::ComputeLinearVelocity(const FLinearMechanic AttemptedMovement, const FVector currentLinearVelocity, const float mass, const float deltaTime,
                                                      bool UseReduction)
{
	FVector velocity = AttemptedMovement.InstantMode
		                   ? AttemptedMovement.Acceleration.GetSafeNormal() * AttemptedMovement.TerminalVelocity
		                   : AttemptedMovement.Acceleration * deltaTime + currentLinearVelocity;
	//Drag
	if (!AttemptedMovement.InstantMode)
	{
		FVector vel = currentLinearVelocity;
		const double velSqr = vel.SquaredLength();
		if (vel.Normalize())
		{
			float drag = (2 * AttemptedMovement.Acceleration.Length() * mass) / (AttemptedMovement.TerminalVelocity * AttemptedMovement.TerminalVelocity);
			if (drag <= 0)
				drag = AttemptedMovement.StaticDrag > 0
					       ? AttemptedMovement.StaticDrag
					       : (2 * AttemptedMovement.TerminalVelocity * mass) / (AttemptedMovement.TerminalVelocity * AttemptedMovement.TerminalVelocity);
			velocity -= (vel * ((velSqr * drag) / (2 * mass)) * deltaTime).GetClampedToMaxSize(velocity.Length());
		}
	}

	if (UseReduction)
		velocity -= currentLinearVelocity;

	return velocity;
}

FVector UModularMoverComponent::ComputeAngularVelocity(FAngularMechanic AttemptedMovement, FVector CurrentAngularVelocity, const FQuat CurrentOrientation, FVector Gravity,
                                                       const float deltaTime, bool UseReduction)
{
	if (!Gravity.Normalize())
		Gravity = FVector::DownVector;
	FAngularMechanic angularPart = AttemptedMovement;
	const float rotSpeed = angularPart.LookOrientation.Length();
	if (angularPart.OrientationDiff.IsIdentity() && angularPart.LookOrientation.Normalize())
	{
		const FQuat targetRot = UKismetMathLibrary::MakeRotFromZX(-Gravity.GetSafeNormal(), angularPart.LookOrientation).Quaternion();
		FQuat desiredOrientation = FQuat::Slerp(CurrentOrientation, targetRot, 1); // FMath::Clamp(rotSpeed * deltaTime, 0, 1));
		desiredOrientation.EnforceShortestArcWith(CurrentOrientation);
		angularPart.OrientationDiff = desiredOrientation * CurrentOrientation.Inverse();
	}

	FVector result = GetAngularVelocity(CurrentOrientation, angularPart, Gravity);
	//result = result.GetSafeNormal() * FMath::DegreesToRadians(result.Length());

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
		linearMovement = ComputeLinearVelocity(movement.Linear, currentLinearVelocity, GetMass(), Delta, true);

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
		angularMovement = ComputeAngularVelocity(movement.Angular, currentAngularVelocity, initialTransform.GetRotation(), movement.Gravity, Delta, true);

		if (DebugMode == EDebugMode::AngularMovement)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(
				                                  TEXT("[AngularVelocity] - Current Vel (%f); Rotation Vel (%f)"),
				                                  FMath::RadiansToDegrees(FMath::RadiansToDegrees(currentAngularVelocity.Length())), FMath::RadiansToDegrees(angularMovement.Length())), true,
			                                  false, FColor::Magenta, 60, "ComputeAngularVelocity");
		}
	}

	//Handle damping values
	Body->LinearDamping = 0;
	Body->AngularDamping = 0;
	Body->UpdateDampingProperties();

	//Wake up and apply movement
	Body->WakeInstance();
	UPhysicToolbox::RigidBodyAddImpulse(Body, linearMovement, true);
	UPhysicToolbox::RigidBodyAddAngularImpulseInRadians(Body, angularMovement, true);
}

bool UModularMoverComponent::GetAngularOrientation(FQuat& Orientation, const FQuat BodyOrientation, const FAngularMechanic angularMechanic, const FVector Gravity)
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
			virtualFwdDir = -virtualFwdDir.Rotation().Quaternion().GetAxisZ();
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

		Orientation = desiredRotator.Quaternion();
	}


	return true;
}

FVector UModularMoverComponent::GetAngularVelocity(const FQuat BodyOrientation, const FAngularMechanic angularMechanic, const FVector Gravity)
{
	FQuat orientation;
	if (!GetAngularOrientation(orientation, BodyOrientation, angularMechanic, Gravity))
		return FVector(0);
	return UPhysicToolbox::OrientationDiffToAngularVelocity(BodyOrientation, orientation);
}

TArray<FMomentum> UModularMoverComponent::GetTrajectory(const int SampleCount, const FMechanicProperties inputMovement, const FMomentum currentMomentum, const float timeStep)
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
			iteration.LinearVelocity = ComputeLinearVelocity(inputMovement.Linear, lastItem.LinearVelocity, iteration.Mass, timeStep);
			iteration.Transform.SetLocation(lastItem.Transform.GetLocation() + iteration.LinearVelocity * timeStep);
		}

		//Angular part
		{
			iteration.AngularVelocity = ComputeAngularVelocity(inputMovement.Angular, lastItem.AngularVelocity, lastItem.Transform.GetRotation(), inputMovement.Gravity, timeStep);
			const FQuat axisAngleRot = FQuat(iteration.AngularVelocity.GetSafeNormal(), iteration.AngularVelocity.Length() * timeStep);
			iteration.Transform.SetRotation(lastItem.Transform.GetRotation() * axisAngleRot);
		}

		result.Add(iteration);
	}

	return result;
}

#pragma endregion
