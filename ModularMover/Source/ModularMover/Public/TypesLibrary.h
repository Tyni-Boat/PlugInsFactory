// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/ObjectMacros.h"
#include "TypesLibrary.generated.h"


#pragma region Enums
#pragma endregion 

#pragma region Structs


// The actual Mover state params.
USTRUCT(BlueprintType)
struct MODULARMOVER_API FBaseMoverStateParams
{
	GENERATED_BODY()

public:
	FBaseMoverStateParams();

	bool IsValid();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FName StateName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	int Priority = -1;
};


#pragma endregion 

#pragma region Classes


// The Modular Mover state's asset class.
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Modular Controller States", abstract)
class MODULARMOVER_API UMoverState : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMoverState();

	template <typename T>
	UFUNCTION(BlueprintNativeEvent, Category = "Modular Mover | States")
	std::enable_if<std::is_base_of_v<FBaseMoverStateParams, T>, T> GetMoverStateParam() const;

	//
	template <typename T>
	std::enable_if<std::is_base_of_v<FBaseMoverStateParams, T>, T> GetMoverStateParam_Implementation() const;
};

#pragma endregion 