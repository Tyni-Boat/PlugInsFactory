// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "ModularMoverComponent.h"

#include "CommonToolboxBPLibrary.h"
#include "Async/Async.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "PhysicsField/PhysicsFieldComponent.h"

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


// Called when the game starts
void UModularMoverComponent::BeginPlay()
{
	Super::BeginPlay();
	// if (FPhysScene* PhysScene = GetWorld()->GetPhysicsScene())
	// 	PhysScene->OnPhysSceneStep.AddUObject(this, &UModularMoverComponent::PhysicsTick);
	if (!UpdatedComponent)
		UpdatedComponent = GetOwner()->GetRootComponent();
	if (UpdatedComponent)
		UpdatedPrimitive = Cast<UPrimitiveComponent>(UpdatedComponent);
	if (UpdatedPrimitive)
	{
		UpdatedPrimitive->SetSimulatePhysics(false);
		//UpdatedPrimitive->bIgnoreRadialForce = true;
		//UpdatedPrimitive->bIgnoreRadialImpulse = true;
		UpdatedPrimitive->SetEnableGravity(false);
		//UpdatedPrimitive->GetBodyInstance()->SetOneWayInteraction(true);
		UpdatedPrimitive->SetUpdateKinematicFromSimulation(true);
	}
	// ...
	if (stateClass)
		state = stateClass.GetDefaultObject();
	if (!state)
		state = NewObject<UBaseMoverMovementMode>();
}


// Called every frame
void UModularMoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UModularMoverComponent::AsyncPhysicsTickComponent(float DeltaTime, float SimTime)
{
	if (UpdatedPrimitive)
	{
		if (const auto BodyInstance = UpdatedPrimitive->GetBodyInstance())
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AsyncPhysicsTickComponent at %f FPS"), UCommonToolboxBPLibrary::GetFPS(DeltaTime)), true, false
			                                  , FColor::Red, 0, "ModeName");
			for (int i = 0; i < 500; i++)
			{
				// FTransform tr = BodyInstance->GetUnrealWorldTransform_AssumesLocked();
				// FCollisionShape shape = UpdatedPrimitive->GetCollisionShape();
				// UWorld* World = GetWorld();
				// state->TestTrace(World, FString::Printf(TEXT("Called from thread")), tr, shape);
			}
		}
	}
	// ...
	Super::AsyncPhysicsTickComponent(DeltaTime, SimTime);
}



#pragma region Physic


#pragma endregion 

#pragma region Flow Mode

// Stand alone mode

#pragma endregion 
