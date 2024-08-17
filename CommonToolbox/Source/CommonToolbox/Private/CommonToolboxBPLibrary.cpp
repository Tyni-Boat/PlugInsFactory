// Copyright Tyni Boat, Inc. All Rights Reserved.

#include "CommonToolboxBPLibrary.h"
#include "CommonToolbox.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Animation/AttributesRuntime.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/HitResult.h"
#include "Kismet/KismetSystemLibrary.h"

UCommonToolboxBPLibrary::UCommonToolboxBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


#pragma region Maths


bool UCommonToolboxBPLibrary::CheckInRange(const FVector2D range, const float value, bool invalidDefault)
{
	if (range.X >= range.Y || !FMath::IsFinite(value))
	{
		return invalidDefault;
	}
	if (range.X <= value && value < range.Y)
		return true;
	return false;
}

double UCommonToolboxBPLibrary::TenPowX(const int exponent)
{
	if (exponent <= 0)
		return 1;
	if (exponent == 1)
		return 10;
	double result = 10;
	for (int i = 1; i < exponent; i++)
		result *= 10;
	return result;
}

double UCommonToolboxBPLibrary::TwoPowX(const int exponent)
{
	if (exponent <= 0)
		return 1;
	if (exponent == 1)
		return 2;
	double result = 2;
	for (int i = 1; i < exponent; i++)
		result *= 2;
	return result;
}


#pragma endregion


#pragma region Conversions

double UCommonToolboxBPLibrary::GetFPS(double deltaTime)
{
	return 1 / deltaTime;
}


int UCommonToolboxBPLibrary::BoolArrayToFlag(const TArray<bool> array)
{
	//Translate to int
	TArray<int> arr;
	for (int i = 0; i < array.Num(); i++)
		arr.Add(array[i] ? 1 : 0);

	int result = 0;
	for (int i = 0; i < arr.Num(); i++)
	{
		result += arr[i] * TwoPowX(i);
	}
	return result;
}


TArray<bool> UCommonToolboxBPLibrary::FlagToBoolArray(int flag)
{
	TArray<int> binary_array;
	TArray<bool> bool_array;
	int n = flag;

	while (n > 0)
	{
		binary_array.Add(n % 2);
		n /= 2;
	}
	for (int i = binary_array.Num() - 1; i >= 0; i--)
		bool_array.Add(binary_array[binary_array.Num() - (i + 1)] >= 1);

	return bool_array;
}


TArray<int> UCommonToolboxBPLibrary::BoolToIndexesArray(const TArray<bool> array)
{
	TArray<int> indexArray;
	//loop the array and add to the int[] i if array[i] == true
	for (int i = 0; i < array.Num(); i++)
		if (array[i])
			indexArray.Add(i);

	return indexArray;
}


TArray<bool> UCommonToolboxBPLibrary::IndexesToBoolArray(const TArray<int> array)
{
	//Get the highest index
	int length = -1;
	for (int i = 0; i < array.Num(); i++)
		if (array[i] > length)
			length = array[i];

	//set the bool[] size
	TArray<bool> bools = TArray<bool>();
	for (int i = 0; i < (length + 1); i++)
		bools.Add(false);
	//loop the array and true at bool[x], where x = arr[i]
	for (int i = 0; i < array.Num(); i++)
	{
		if (!bools.IsValidIndex(array[i]))
			continue;
		bools[array[i]] = true;
	}

	return bools;
}


int UCommonToolboxBPLibrary::IndexToFlag(const int index)
{
	if (index < 0)
		return 0;
	return TwoPowX(index);
}


#pragma endregion

#pragma region Objects
#pragma endregion

#pragma region Vectors

FVector UCommonToolboxBPLibrary::VectorCone(const FVector inVector, const FVector normal, const float alphaAngle)
{
	FVector n = normal;
	if (!n.Normalize())
		return inVector;
	const float angle = FMath::Clamp(alphaAngle, 0, 180);
	if (angle == 90)
	{
		if ((inVector | n) <= 0)
			return FVector::VectorPlaneProject(inVector, n);
		return inVector;
	}
	FVector v = inVector;
	if (!v.Normalize())
		return inVector;
	const float vectorLenght = inVector.Length();
	const float cosine = FMath::Cos(FMath::DegreesToRadians(angle));
	const float sine = FMath::Sin(FMath::DegreesToRadians(angle));
	FVector cosineVector = v.ProjectOnToNormal(n);
	FVector sineVector = FVector::VectorPlaneProject(v, n);
	if (angle < 90)
	{
		cosineVector = cosineVector.GetSafeNormal() * FMath::Clamp(cosineVector.Length(), FMath::Abs(cosine), 1);
		sineVector = sineVector.GetSafeNormal() * FMath::Clamp(sineVector.Length(), 0, FMath::Abs(sine));
	}
	else
	{
		cosineVector = cosineVector.GetSafeNormal() * FMath::Clamp(cosineVector.Length(), 0, FMath::Abs(cosine));
		sineVector = sineVector.GetSafeNormal() * FMath::Clamp(sineVector.Length(), FMath::Abs(sine), 1);
	}

	return (sineVector + cosineVector).GetSafeNormal() * vectorLenght;
}

bool UCommonToolboxBPLibrary::IsVectorCone(const FVector inVector, const FVector normal, const float alphaAngle)
{
	FVector n = normal;
	if (!n.Normalize())
		return false;
	const float angle = FMath::Clamp(alphaAngle, 0, 180);
	const float cosine = (inVector.GetSafeNormal() | n);
	if (FMath::RadiansToDegrees(FMath::Acos(cosine)) <= angle)
		return true;
	return false;
}

FVector UCommonToolboxBPLibrary::Project2DInputs(FVector2D input, FTransform transformRelative, FVector planeNormal)
{
	FVector direction = FVector(0);
	FVector fwd = transformRelative.GetRotation().GetForwardVector();
	FVector rht = transformRelative.GetRotation().GetRightVector();
	if (planeNormal.Length() > 0 && planeNormal.Normalize())
	{
		fwd = FVector::VectorPlaneProject(fwd, planeNormal).GetSafeNormal();
		rht = FVector::VectorPlaneProject(rht, planeNormal).GetSafeNormal();
	}
	const FVector compositeRhs = rht * input.X;
	const FVector compositeFwd = fwd * input.Y;
	direction = compositeFwd + compositeRhs;
	return direction;
}

FVector UCommonToolboxBPLibrary::GetSnapOnSurfaceVector(const FVector point, const FHitResult surface, const FVector axis)
{
	if (!surface.Component.IsValid())
		return FVector(0);
	const FVector snapDirection = (-axis).GetSafeNormal();
	const FVector hitPoint = surface.ImpactPoint;
	FVector snapVector = hitPoint - point;
	if (snapDirection.SquaredLength() > 0)
		snapVector = snapVector.ProjectOnToNormal(snapDirection);
	return snapVector;
}

#pragma endregion

#pragma region Arrays
#pragma endregion

#pragma region Animations

void UCommonToolboxBPLibrary::ExtractLocalSpacePose(const UAnimSequenceBase* Animation, const FBoneContainer& BoneContainer, float Time, bool bExtractRootMotion, FCompactPose& OutPose)
{
	OutPose.SetBoneContainer(&BoneContainer);

	FBlendedCurve Curve;
	Curve.InitFrom(BoneContainer);

	FAnimExtractContext Context(static_cast<double>(Time), bExtractRootMotion);

	UE::Anim::FStackAttributeContainer Attributes;
	FAnimationPoseData AnimationPoseData(OutPose, Curve, Attributes);
	if (const UAnimSequence* AnimSequence = Cast<UAnimSequence>(Animation))
	{
		AnimSequence->GetBonePose(AnimationPoseData, Context);
	}
	else if (const UAnimMontage* AnimMontage = Cast<UAnimMontage>(Animation))
	{
		const FAnimTrack& AnimTrack = AnimMontage->SlotAnimTracks[0].AnimTrack;
		AnimTrack.GetAnimationPose(AnimationPoseData, Context);
	}
}

void UCommonToolboxBPLibrary::ExtractComponentSpacePose(const UAnimSequenceBase* Animation, const FBoneContainer& BoneContainer, float Time, bool bExtractRootMotion,
                                                        FCSPose<FCompactPose>& OutPose)
{
	FCompactPose Pose;
	ExtractLocalSpacePose(Animation, BoneContainer, Time, bExtractRootMotion, Pose);
	OutPose.InitPose(MoveTemp(Pose));
}

FTransform UCommonToolboxBPLibrary::ExtractRootMotionFromAnimation(const UAnimSequenceBase* Animation, float StartTime, float EndTime, TArray<FTransform>* stepArray)
{
	if (stepArray)
		stepArray->Empty();
	if (const UAnimMontage* Anim = Cast<UAnimMontage>(Animation))
	{
		// This is identical to UAnimMontage::ExtractRootMotionFromTrackRange and UAnimCompositeBase::ExtractRootMotionFromTrack but ignoring bEnableRootMotion
		// so we can extract root motion from the montage even if that flag is set to false in the AnimSequence(s)

		FRootMotionMovementParams AccumulatedRootMotionParams;

		if (Anim->SlotAnimTracks.Num() > 0)
		{
			const FAnimTrack& RootMotionAnimTrack = Anim->SlotAnimTracks[0].AnimTrack;

			TArray<FRootMotionExtractionStep> RootMotionExtractionSteps;
			RootMotionAnimTrack.GetRootMotionExtractionStepsForTrackRange(RootMotionExtractionSteps, StartTime, EndTime);

			for (const FRootMotionExtractionStep& CurStep : RootMotionExtractionSteps)
			{
				if (CurStep.AnimSequence)
				{
					if (stepArray)
					{
						FRootMotionMovementParams stepRM;
						stepRM.Accumulate(CurStep.AnimSequence->ExtractRootMotionFromRange(CurStep.StartPosition, CurStep.EndPosition));
						stepArray->Add(stepRM.GetRootMotionTransform());
					}
					AccumulatedRootMotionParams.Accumulate(CurStep.AnimSequence->ExtractRootMotionFromRange(CurStep.StartPosition, CurStep.EndPosition));
				}
			}
		}

		return AccumulatedRootMotionParams.GetRootMotionTransform();
	}

	if (const UAnimSequence* Anim = Cast<UAnimSequence>(Animation))
	{
		return Anim->ExtractRootMotionFromRange(StartTime, EndTime);
	}

	return FTransform::Identity;
}

FTransform UCommonToolboxBPLibrary::ExtractRootTransformFromAnimation(const UAnimSequenceBase* Animation, float Time)
{
	if (const UAnimMontage* AnimMontage = Cast<UAnimMontage>(Animation))
	{
		if (const FAnimSegment* Segment = AnimMontage->SlotAnimTracks[0].AnimTrack.GetSegmentAtTime(Time))
		{
			if (const UAnimSequence* AnimSequence = Cast<UAnimSequence>(Segment->GetAnimReference()))
			{
				const float AnimSequenceTime = Segment->ConvertTrackPosToAnimPos(Time);
				return AnimSequence->ExtractRootTrackTransform(AnimSequenceTime, nullptr);
			}
		}
	}
	else if (const UAnimSequence* AnimSequence = Cast<UAnimSequence>(Animation))
	{
		return AnimSequence->ExtractRootTrackTransform(Time, nullptr);
	}

	return FTransform::Identity;
}

double UCommonToolboxBPLibrary::GetMontageCurrentWeight(const UAnimInstance* AnimInstance, const UAnimMontage* Montage)
{
	if (!AnimInstance)
		return 0;
	const auto MontageInstance = Montage ? AnimInstance->GetInstanceForMontage(Montage) : AnimInstance->GetActiveMontageInstance();
	if (!MontageInstance)
		return 0;
	return MontageInstance->GetWeight();
}


#pragma endregion

#pragma region Physic


bool UCommonToolboxBPLibrary::CollisionShapeEquals(const FCollisionShape shapeA, const FCollisionShape shapeB)
{
	if (shapeA.ShapeType != shapeB.ShapeType)
		return false;
	return shapeA.GetExtent() == shapeB.GetExtent();
}


FVector UCommonToolboxBPLibrary::GetKineticEnergy(const FVector velocity, const float mass, const double distanceTraveled)
{
	const FVector momentum = velocity * mass;
	const FVector kinematicEnergy = momentum.GetSafeNormal() * (momentum.SquaredLength() / (2 * mass));
	const FVector force = kinematicEnergy / distanceTraveled;
	return force;
}


bool UCommonToolboxBPLibrary::ComponentTraceMulti_internal(FCollisionShape Shape, ECollisionChannel Channel, TArray<FExpandedHitResult>& outHits, FVector position, FVector direction,
                                                           FQuat rotation, bool traceComplex,
                                                           FCollisionQueryParams& queryParams, ESurfaceTraceHitType offsetFilter, float PenetrationStep) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ComponentTraceCastMulti");
	if (!GetWorld())
		return false;
	queryParams.bTraceComplex = traceComplex;
	queryParams.bReturnPhysicalMaterial = true;
	const auto shape = Shape;
	const auto channel = Channel;
	FCollisionResponseParams response = FCollisionResponseParams::DefaultResponseParam;
	response.CollisionResponse.SetAllChannels(ECollisionResponse::ECR_Block);

	constexpr int maxIterations = 64;
	TArray<FHitResult> loopHits;
	const FVector endPoint = position + direction;
	FCollisionQueryParams loopQueryParams = queryParams;
	for (int i = 0; i < maxIterations; i++)
	{
		const bool result = GetWorld()->SweepMultiByChannel(loopHits, position, endPoint, rotation, channel, shape, loopQueryParams);
		for (int j = 0; j < loopHits.Num(); j++)
		{
			auto queryType = loopHits[j].Component->GetCollisionResponseToChannel(channel);
			outHits.Add(FExpandedHitResult(loopHits[j], queryType));
			ESurfaceTraceHitType _offset = ESurfaceTraceHitType::NormalHit;
			const float abs_penetrationStep = FMath::Abs(PenetrationStep);
			FVector penetrationIniLocation = loopHits[j].ImpactPoint;
			FVector awayOffset = FVector::VectorPlaneProject(loopHits[j].ImpactPoint - position, direction.GetSafeNormal());

			if (queryType == ECR_Block && direction.SquaredLength() > 0)
			{
				const auto respParam = FCollisionResponseParams::DefaultResponseParam;
				const auto objParam = FCollisionObjectQueryParams::DefaultObjectQueryParam;

				//Inward chk
				if (_offset == ESurfaceTraceHitType::NormalHit && (offsetFilter == ESurfaceTraceHitType::InnerHit || offsetFilter == ESurfaceTraceHitType::MAX))
				{
					FHitResult inwardHit;
					FVector pt = loopHits[j].ImpactPoint - awayOffset.GetSafeNormal() * 0.125;
					if (loopHits[j].GetComponent()->LineTraceComponent(inwardHit, pt, pt + direction, channel, loopQueryParams, respParam, objParam))
					{
						_offset = ESurfaceTraceHitType::InnerHit;
						penetrationIniLocation = inwardHit.ImpactPoint;
						outHits.Add(FExpandedHitResult(inwardHit, queryType, _offset));
					}
				}

				//Outward chk
				if (_offset == ESurfaceTraceHitType::NormalHit && (offsetFilter == ESurfaceTraceHitType::OuterHit || offsetFilter == ESurfaceTraceHitType::MAX))
				{
					FHitResult outwardHit;
					FVector pt = loopHits[j].ImpactPoint + awayOffset.GetSafeNormal() * 0.125;
					if (loopHits[j].GetComponent()->LineTraceComponent(outwardHit, pt, pt + direction, channel, loopQueryParams, respParam, objParam))
					{
						_offset = ESurfaceTraceHitType::OuterHit;
						penetrationIniLocation = outwardHit.ImpactPoint;
						outHits.Add(FExpandedHitResult(outwardHit, queryType, _offset));
					}
				}

				if (abs_penetrationStep > 0)
				{
					int maxSubSurfaceHit = 5;
					FVector penetrationPt = penetrationIniLocation + direction.GetSafeNormal() * abs_penetrationStep;
					FHitResult deepHit;
					while (loopHits[j].GetComponent()->LineTraceComponent(deepHit, penetrationPt, penetrationPt + direction, channel, loopQueryParams, respParam, objParam))
					{
						if (deepHit.Distance < abs_penetrationStep)
							break;
						if (!deepHit.bStartPenetrating)
						{
							outHits.Add(FExpandedHitResult(deepHit, queryType, _offset, deepHit.Distance + (penetrationPt - penetrationIniLocation).Length()));
						}
						penetrationPt += direction.GetSafeNormal() * FMath::Max(abs_penetrationStep, deepHit.Distance);
						maxSubSurfaceHit--;
						if (maxSubSurfaceHit <= 0)
							break;
					}
				}
			}
			loopQueryParams.AddIgnoredComponent(loopHits[j].GetComponent());
		}

		if (!result)
			break;
	}
	queryParams.ClearIgnoredActors();

	return outHits.Num() > 0;
}

#pragma endregion


#pragma region Debug


void UCommonToolboxBPLibrary::DrawDebugCircleOnHit(const FHitResult MyStructRef, float radius, FLinearColor color, float duration, float thickness, bool showImpactAxis)
{
	if (!MyStructRef.Component.IsValid())
		return;
	FVector up = MyStructRef.Normal;
	FVector axisUp = MyStructRef.ImpactNormal;
	if (!up.Normalize())
		return;
	const FVector hitPoint = MyStructRef.ImpactPoint + up * 0.01;
	FVector right = axisUp.Rotation().Quaternion().GetAxisY();
	FVector forward = FVector::CrossProduct(right, axisUp);
	if (showImpactAxis && axisUp.Normalize())
	{
		FVector::CreateOrthonormalBasis(forward, right, axisUp);
		UKismetSystemLibrary::DrawDebugArrow(MyStructRef.GetComponent(), hitPoint, hitPoint + axisUp * radius, (radius * 0.25), FColor::Blue, duration, thickness);
		UKismetSystemLibrary::DrawDebugArrow(MyStructRef.GetComponent(), hitPoint, hitPoint + forward * (radius * 0.5), (radius * 0.25), FColor::Red, duration, thickness);
		UKismetSystemLibrary::DrawDebugArrow(MyStructRef.GetComponent(), hitPoint, hitPoint + right * (radius * 0.5), (radius * 0.25), FColor::Green, duration, thickness);
	}
	right = up.Rotation().Quaternion().GetAxisY();
	forward = FVector::CrossProduct(right, up);
	FVector::CreateOrthonormalBasis(forward, right, up);
	UKismetSystemLibrary::DrawDebugCircle(MyStructRef.GetComponent(), hitPoint, radius, 32,
	                                      color, duration, thickness, right, forward);
}

#pragma endregion
