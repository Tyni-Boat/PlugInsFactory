// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CollisionShape.h"
#include "Engine/DataAsset.h"
#include "UObject/ObjectMacros.h"
#include "TypesLibrary.generated.h"


#pragma region Enums
#pragma endregion 

#pragma region Structs

#pragma endregion 

#pragma region Classes


// The Modular Mover state's asset class.
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Modular Controller States", abstract)
class MODULARMOVER_API UBaseMoverState : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBaseMoverState();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FName StateName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	int Priority = -1;


public:

	UFUNCTION(BlueprintPure, Category="Modular Mover | Mover State", meta = (CompactNodeTitle = "ValidState", BlueprintThreadSafe))
	bool IsValid() const;

	void TestTrace(FString message, FTransform tr, FCollisionShape shape) const;
};

#pragma endregion 