// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"
#include "Components/ActorComponent.h"
#include "GameFramework/MovementComponent.h"
#include "ModularMoverComponent.generated.h"

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Custom physics Delegate
	FCalculateCustomPhysics OnCalculateCustomPhysics;
	void PhysicsTick(float DeltaTime, FBodyInstance* BodyInstance);
		
};
