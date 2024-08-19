// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"
#include "TypesLibrary.h"
#include "Components/ActorComponent.h"
#include "GameFramework/MovementComponent.h"
#include "ModularMoverComponent.generated.h"



DECLARE_DELEGATE_OneParam(FOnMoveDelegate, FVector)



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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<UBaseMoverMovementMode> stateClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default", Transient)
	UBaseMoverMovementMode* state;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void AsyncPhysicsTickComponent(float DeltaTime, float SimTime) override;


#pragma region Physic


#pragma endregion 

#pragma region Flow Mode

	// Stand alone mode

#pragma endregion 
		
};
