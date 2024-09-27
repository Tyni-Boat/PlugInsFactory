// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AnimToolbox.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLBOX_API UAnimToolbox : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void ExtractLocalSpacePose(const UAnimSequenceBase* Animation, const FBoneContainer& BoneContainer, float Time, bool bExtractRootMotion, FCompactPose& OutPose);

	static void ExtractComponentSpacePose(const UAnimSequenceBase* Animation, const FBoneContainer& BoneContainer, float Time, bool bExtractRootMotion, FCSPose<FCompactPose>& OutPose);

	static FTransform ExtractRootMotionFromAnimation(const UAnimSequenceBase* Animation, float StartTime, float EndTime, TArray<FTransform>* stepArray = nullptr);

	static FTransform ExtractRootTransformFromAnimation(const UAnimSequenceBase* Animation, float Time);

	// Get the weight of the montage specified or of the active montage if NULL Montage.
	UFUNCTION(BlueprintPure, Category = "Toolbox|Animation")
	static double GetMontageCurrentWeight(const UAnimInstance* AnimInstance, const UAnimMontage* Montage);

	// Extract root motion from skeletal mesh
	UFUNCTION(BlueprintPure, Category = "Toolbox|Animation")
	static FTransform ExtractRootMotion(USkeletalMeshComponent* SkeletalMesh, float delta);
};
