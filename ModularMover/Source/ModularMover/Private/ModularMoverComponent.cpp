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

void UModularMoverComponent::MoveBody(FBodyInstance* Body, const FMechanicProperties movement, const float Delta)
{
	if (!Body)
		return;
	const float deltaTime = Delta;
	const FTransform initialTransform = UPhysicToolbox::GetRigidBodyTransform(Body);
	const FVector currentLinearVelocity = UPhysicToolbox::GetRigidBodyLinearVelocity(Body);
	const FVector currentAngularVelocity = UPhysicToolbox::GetRigidBodyAngularVelocity(Body);
	const float fps = UConversionToolbox::DeltaTimeToFPS(deltaTime);

	//
	FVector linearMovement = FVector(0);
	bool linearAccChange = false;
	float linearDamping = 0;
	FVector angularMovement = FVector(0);
	bool angularAccChange = false;
	float angularDamping = 0;

	//Linear Part
	{
		if (movement.Linear.InstantMode || movement.Linear.Acceleration.Length() >= movement.Linear.TerminalVelocity / deltaTime)
		{
			//Instant mode
			FVector acceleration = movement.Linear.Acceleration.GetSafeNormal() * movement.Linear.TerminalVelocity;
			linearDamping = acceleration.SquaredLength() > 0 ? 0 : movement.Linear.StaticDrag;
			linearAccChange = true;
			const float moveDirection = FMath::Clamp(currentLinearVelocity.GetSafeNormal() | acceleration.GetSafeNormal(), 0, 1);
			acceleration -= currentLinearVelocity;
			linearMovement = acceleration;


			if (DebugMode == EDebugMode::LinearMovement)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(
					                                  TEXT("[Instant Mode] - Linear Vel (%f); Linear Damping (%f); Move Dir (%f); Movement Vel (%f)"),
					                                  currentLinearVelocity.Length(), Body->LinearDamping, moveDirection, acceleration.Length()), true,
				                                  false, FColor::Magenta, 60, "ModeName2");
			}
		}
		else
		{
			//Progressive Mode
			FVector acceleration = movement.Linear.Acceleration;
			const float accRatio = movement.Linear.Acceleration.Length() / 100;
			const float linearLimit = movement.Linear.TerminalVelocity - movement.Linear.TerminalVelocity * deltaTime + accRatio * (1 - deltaTime);

			const float moveDirection = FMath::Clamp(currentLinearVelocity.GetSafeNormal() | acceleration.GetSafeNormal(), 0, 1);
			const float forceRatio = movement.Linear.TerminalVelocity > 0 ? FMath::Clamp(accRatio, 1, TNumericLimits<float>::Max()) : 1;
			const float fpsRatio = (linearLimit / fps);
			linearDamping = acceleration.SquaredLength() > 0 ? ((1 - deltaTime) / fpsRatio) * forceRatio : movement.Linear.StaticDrag;
			linearAccChange = false;
			linearMovement = acceleration;

			if (DebugMode == EDebugMode::LinearMovement)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(
					                                  TEXT(
						                                  "[Progressive Mode] - Linear Vel (%f); Linear Damping (%f); Terminal Vel (%f); Move Dir (%f); FPS Ratio (%f); Movement Vel (%f), Acc Ratio (%f)"),
					                                  currentLinearVelocity.Length(), Body->LinearDamping, linearLimit, moveDirection, fpsRatio, acceleration.Length(), forceRatio), true,
				                                  false, FColor::Magenta, 60, "ModeName2");
			}
		}
	}


	//Angular Part
	{
		FAngularMechanic angularPart = movement.Angular;

		if (movement.Angular.TurnSpeed <= 0 || movement.Angular.TurnSpeed >= 360)
		{
			//Instant Mode
			FVector angVel = GetAngularVelocity(Body, angularPart, movement.Gravity, deltaTime);
			angularMovement = angVel - currentAngularVelocity;
			angularAccChange = true;
			angularDamping = 0;

			if (DebugMode == EDebugMode::AngularMovement)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(
					                                  TEXT("[Instant Mode] - Last Angular Vel (%f); Angular Damping (%f); Look Dir (%s)"),
					                                  FMath::RadiansToDegrees(currentAngularVelocity.Length()), Body->AngularDamping,
					                                  *angularPart.LookDirection.ToCompactString()), true,
				                                  false, FColor::Magenta, 60, "ModeName2");
			}
		}
		else
		{
			//Progressive Mode
			FVector angVel = GetAngularVelocity(Body, angularPart, movement.Gravity, deltaTime);
			angularAccChange = false;
			angularMovement = angVel * fps * angularPart.LookDirection.Length();
			angularDamping = FAlphaBlend::AlphaToBlendOption(FMath::GetMappedRangeValueClamped(TRange<double>(180.0, 0.0), TRange<double>(0.0, 1), angVel.Length()),
			                                                 EAlphaBlendOption::ExpIn) * fps * FMath::Clamp(currentAngularVelocity.Length() * 0.1, 1, TNumericLimits<float>::Max());
			// * FMath::Clamp(angularPart.LookDirection.Length() / 45, 1, TNumericLimits<float>::Max());

			if (DebugMode == EDebugMode::AngularMovement)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(
					                                  TEXT("[Instant Mode] - Last Angular Vel (%f); Angular Damping (%f); Look Dir (%s); Angular Vel (%f)"),
					                                  FMath::RadiansToDegrees(currentAngularVelocity.Length()), Body->AngularDamping,
					                                  *angularPart.LookDirection.ToCompactString(), angVel.Length()), true,
				                                  false, FColor::Magenta, 60, "ModeName2");
			}
		}
	}

	//Handle damping values
	Body->LinearDamping = linearDamping;
	Body->AngularDamping = angularDamping;
	Body->UpdateDampingProperties();

	//Wake up and apply movement
	Body->WakeInstance();
	//UPhysicToolbox::RigidBodyAddImpulse(Body, linearMovement, linearAccChange);
	UPhysicToolbox::RigidBodyAddAngularImpulseInRadians(Body, angularMovement, angularAccChange);
}

bool UModularMoverComponent::GetAngularOrientation(FQuat& Orientation, const FBodyInstance* Body, const FAngularMechanic angularMechanic, const FVector Gravity, const float inDelta) const
{
	if (!Body)
		return false;
	const FTransform initialTransform = UPhysicToolbox::GetRigidBodyTransform(Body);
	Orientation = initialTransform.GetRotation();

	FVector gravityUp = -Gravity;
	if (!gravityUp.Normalize())
		gravityUp = FVector::UpVector;

	//Handle surfaces rotation
	{
		// const FVector surfaceRotVel = UFunctionLibrary::GetAverageSurfaceAngularSpeed(angularMechanic);
		// output.LookDirection *= FQuat(surfaceRotVel.GetSafeNormal(), FMath::DegreesToRadians(surfaceRotVel.Length()) * inDelta);
	}

	//LookDirection
	{
		FVector virtualFwdDir = FVector::VectorPlaneProject(angularMechanic.LookDirection.SquaredLength() > 0
			                                                    ? angularMechanic.LookDirection.GetSafeNormal()
			                                                    : initialTransform.GetRotation().Vector(), gravityUp);
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
			if (DebugMode == EDebugMode::AngularMovement)
			{
				UKismetSystemLibrary::PrintString(
					this, FString::Printf(TEXT("Cannot normalize right vector: up = %s, fwd= %s"), *gravityUp.ToCompactString(), *virtualFwdDir.ToCompactString()), true,
					false, FColor::Yellow, 60, "RotError");
			}
			return false;
		}
		const FRotator desiredRotator = UKismetMathLibrary::MakeRotFromZX(gravityUp, virtualFwdDir);
		virtualFwdDir = desiredRotator.Quaternion().GetAxisX();
		virtualRightDir = desiredRotator.Quaternion().GetAxisY();
		
		Orientation = desiredRotator.Quaternion() * RotationOffset.Quaternion();
	}


	return true;
}

FVector UModularMoverComponent::GetAngularVelocity(const FBodyInstance* Body, const FAngularMechanic angularMechanic, const FVector Gravity, const float inDelta) const
{
	FQuat orientation;
	if (!GetAngularOrientation(orientation, Body, angularMechanic, Gravity, inDelta))
		return FVector(0);

	FVector output = FVector(0);
	const FTransform initialTransform = UPhysicToolbox::GetRigidBodyTransform(Body);

	orientation.EnforceShortestArcWith(initialTransform.GetRotation());
	FQuat diff = orientation * initialTransform.GetRotation().Inverse();
	diff.Normalize();
	if(!diff.IsNormalized())
		return FVector(0);
	FVector axis;
	float angle;
	diff.ToAxisAndAngle(axis, angle);

	
	UKismetSystemLibrary::DrawDebugArrow(this, initialTransform.GetLocation(), initialTransform.GetLocation() + axis * 250, 200, FColor::Red, 0, 3);
	//UKismetSystemLibrary::DrawDebugArrow(this, initialTransform.GetLocation(), initialTransform.GetLocation() + diff.GetRightVector() * 250, 200, FColor::Green, 0, 3);
	//UKismetSystemLibrary::DrawDebugArrow(this, initialTransform.GetLocation(), initialTransform.GetLocation() + diff.GetUpVector() * 250, 200, FColor::Blue, 0, 3);
	
	angle = FMath::RadiansToDegrees(angle);
	//if (!FMath::IsNearlyZero(angle, inDelta * 10))
		output = axis * angle;

	return output;
}

#pragma endregion
