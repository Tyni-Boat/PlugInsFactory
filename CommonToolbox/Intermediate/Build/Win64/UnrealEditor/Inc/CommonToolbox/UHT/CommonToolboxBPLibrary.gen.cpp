// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "CommonToolbox/Public/CommonToolboxBPLibrary.h"
#include "Runtime/Engine/Classes/Engine/HitResult.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCommonToolboxBPLibrary() {}

// Begin Cross Module References
COMMONTOOLBOX_API UClass* Z_Construct_UClass_UCommonToolboxBPLibrary();
COMMONTOOLBOX_API UClass* Z_Construct_UClass_UCommonToolboxBPLibrary_NoRegister();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FLinearColor();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FTransform();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector2D();
ENGINE_API UClass* Z_Construct_UClass_UAnimInstance_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UAnimMontage_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary();
ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FHitResult();
UPackage* Z_Construct_UPackage__Script_CommonToolbox();
// End Cross Module References

// Begin Class UCommonToolboxBPLibrary Function BoolArrayToFlag
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics
{
	struct CommonToolboxBPLibrary_eventBoolArrayToFlag_Parms
	{
		TArray<bool> array;
		int32 ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Conversions" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Convert from a bool array to an integer flag. useful to serialize indexes in an array.\n" },
#endif
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Convert from a bool array to an integer flag. useful to serialize indexes in an array." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_array_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FBoolPropertyParams NewProp_array_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_array;
	static const UECodeGen_Private::FIntPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::NewProp_array_Inner = { "array", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::NewProp_array = { "array", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventBoolArrayToFlag_Parms, array), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_array_MetaData), NewProp_array_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventBoolArrayToFlag_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::NewProp_array_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::NewProp_array,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "BoolArrayToFlag", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::CommonToolboxBPLibrary_eventBoolArrayToFlag_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::CommonToolboxBPLibrary_eventBoolArrayToFlag_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execBoolArrayToFlag)
{
	P_GET_TARRAY(bool,Z_Param_array);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(int32*)Z_Param__Result=UCommonToolboxBPLibrary::BoolArrayToFlag(Z_Param_array);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function BoolArrayToFlag

// Begin Class UCommonToolboxBPLibrary Function BoolToIndexesArray
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics
{
	struct CommonToolboxBPLibrary_eventBoolToIndexesArray_Parms
	{
		TArray<bool> array;
		TArray<int32> ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Conversions" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Convert a bool array to an index array\n" },
#endif
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Convert a bool array to an index array" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_array_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FBoolPropertyParams NewProp_array_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_array;
	static const UECodeGen_Private::FIntPropertyParams NewProp_ReturnValue_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::NewProp_array_Inner = { "array", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::NewProp_array = { "array", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventBoolToIndexesArray_Parms, array), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_array_MetaData), NewProp_array_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::NewProp_ReturnValue_Inner = { "ReturnValue", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventBoolToIndexesArray_Parms, ReturnValue), EArrayPropertyFlags::None, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::NewProp_array_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::NewProp_array,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::NewProp_ReturnValue_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "BoolToIndexesArray", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::CommonToolboxBPLibrary_eventBoolToIndexesArray_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::CommonToolboxBPLibrary_eventBoolToIndexesArray_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execBoolToIndexesArray)
{
	P_GET_TARRAY(bool,Z_Param_array);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(TArray<int32>*)Z_Param__Result=UCommonToolboxBPLibrary::BoolToIndexesArray(Z_Param_array);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function BoolToIndexesArray

// Begin Class UCommonToolboxBPLibrary Function CheckInRange
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics
{
	struct CommonToolboxBPLibrary_eventCheckInRange_Parms
	{
		FVector2D range;
		float value;
		bool invalidDefault;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Math" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Check if value is in range (rangeX <= Value < rangeY). if range is invalid (rangeX >= rangeY) return invalidDefault\n" },
#endif
		{ "CPP_Default_invalidDefault", "false" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Check if value is in range (rangeX <= Value < rangeY). if range is invalid (rangeX >= rangeY) return invalidDefault" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_range_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_value_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_range;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_value;
	static void NewProp_invalidDefault_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_invalidDefault;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_range = { "range", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventCheckInRange_Parms, range), Z_Construct_UScriptStruct_FVector2D, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_range_MetaData), NewProp_range_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_value = { "value", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventCheckInRange_Parms, value), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_value_MetaData), NewProp_value_MetaData) };
void Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_invalidDefault_SetBit(void* Obj)
{
	((CommonToolboxBPLibrary_eventCheckInRange_Parms*)Obj)->invalidDefault = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_invalidDefault = { "invalidDefault", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CommonToolboxBPLibrary_eventCheckInRange_Parms), &Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_invalidDefault_SetBit, METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((CommonToolboxBPLibrary_eventCheckInRange_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CommonToolboxBPLibrary_eventCheckInRange_Parms), &Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_range,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_value,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_invalidDefault,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "CheckInRange", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::CommonToolboxBPLibrary_eventCheckInRange_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14822401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::CommonToolboxBPLibrary_eventCheckInRange_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execCheckInRange)
{
	P_GET_STRUCT(FVector2D,Z_Param_range);
	P_GET_PROPERTY(FFloatProperty,Z_Param_value);
	P_GET_UBOOL(Z_Param_invalidDefault);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=UCommonToolboxBPLibrary::CheckInRange(Z_Param_range,Z_Param_value,Z_Param_invalidDefault);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function CheckInRange

// Begin Class UCommonToolboxBPLibrary Function DrawDebugCircleOnHit
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics
{
	struct CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms
	{
		FHitResult MyStructRef;
		float radius;
		FLinearColor color;
		float duration;
		float thickness;
		bool showImpactAxis;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "AdvancedDisplay", "1" },
		{ "Category", "Common Toolbox | Debug" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Draw a debug circle at the hit point on a surface.\n" },
#endif
		{ "CPP_Default_color", "(R=1.000000,G=1.000000,B=1.000000,A=1.000000)" },
		{ "CPP_Default_duration", "0.000000" },
		{ "CPP_Default_radius", "40.000000" },
		{ "CPP_Default_showImpactAxis", "false" },
		{ "CPP_Default_thickness", "1.000000" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Draw a debug circle at the hit point on a surface." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MyStructRef_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_MyStructRef;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_radius;
	static const UECodeGen_Private::FStructPropertyParams NewProp_color;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_duration;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_thickness;
	static void NewProp_showImpactAxis_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_showImpactAxis;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_MyStructRef = { "MyStructRef", nullptr, (EPropertyFlags)0x0010008000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms, MyStructRef), Z_Construct_UScriptStruct_FHitResult, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MyStructRef_MetaData), NewProp_MyStructRef_MetaData) }; // 4100991306
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_radius = { "radius", nullptr, (EPropertyFlags)0x0010040000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms, radius), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_color = { "color", nullptr, (EPropertyFlags)0x0010040000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms, color), Z_Construct_UScriptStruct_FLinearColor, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_duration = { "duration", nullptr, (EPropertyFlags)0x0010040000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms, duration), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_thickness = { "thickness", nullptr, (EPropertyFlags)0x0010040000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms, thickness), METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_showImpactAxis_SetBit(void* Obj)
{
	((CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms*)Obj)->showImpactAxis = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_showImpactAxis = { "showImpactAxis", nullptr, (EPropertyFlags)0x0010040000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms), &Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_showImpactAxis_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_MyStructRef,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_radius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_color,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_duration,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_thickness,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::NewProp_showImpactAxis,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "DrawDebugCircleOnHit", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04822401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::CommonToolboxBPLibrary_eventDrawDebugCircleOnHit_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execDrawDebugCircleOnHit)
{
	P_GET_STRUCT(FHitResult,Z_Param_MyStructRef);
	P_GET_PROPERTY(FFloatProperty,Z_Param_radius);
	P_GET_STRUCT(FLinearColor,Z_Param_color);
	P_GET_PROPERTY(FFloatProperty,Z_Param_duration);
	P_GET_PROPERTY(FFloatProperty,Z_Param_thickness);
	P_GET_UBOOL(Z_Param_showImpactAxis);
	P_FINISH;
	P_NATIVE_BEGIN;
	UCommonToolboxBPLibrary::DrawDebugCircleOnHit(Z_Param_MyStructRef,Z_Param_radius,Z_Param_color,Z_Param_duration,Z_Param_thickness,Z_Param_showImpactAxis);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function DrawDebugCircleOnHit

// Begin Class UCommonToolboxBPLibrary Function FlagToBoolArray
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics
{
	struct CommonToolboxBPLibrary_eventFlagToBoolArray_Parms
	{
		int32 flag;
		TArray<bool> ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Conversions" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Convert from an integer flag to an bool array. useful to deserialize indexes in an array.\n" },
#endif
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Convert from an integer flag to an bool array. useful to deserialize indexes in an array." },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_flag;
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::NewProp_flag = { "flag", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventFlagToBoolArray_Parms, flag), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::NewProp_ReturnValue_Inner = { "ReturnValue", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventFlagToBoolArray_Parms, ReturnValue), EArrayPropertyFlags::None, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::NewProp_flag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::NewProp_ReturnValue_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "FlagToBoolArray", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::CommonToolboxBPLibrary_eventFlagToBoolArray_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::CommonToolboxBPLibrary_eventFlagToBoolArray_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execFlagToBoolArray)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_flag);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(TArray<bool>*)Z_Param__Result=UCommonToolboxBPLibrary::FlagToBoolArray(Z_Param_flag);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function FlagToBoolArray

// Begin Class UCommonToolboxBPLibrary Function GetFPS
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics
{
	struct CommonToolboxBPLibrary_eventGetFPS_Parms
	{
		double deltaTime;
		double ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Function Library | Conversions" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Get the number of frame per seconds from a delta time\n" },
#endif
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Get the number of frame per seconds from a delta time" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FDoublePropertyParams NewProp_deltaTime;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::NewProp_deltaTime = { "deltaTime", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetFPS_Parms, deltaTime), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetFPS_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::NewProp_deltaTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "GetFPS", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::CommonToolboxBPLibrary_eventGetFPS_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::CommonToolboxBPLibrary_eventGetFPS_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execGetFPS)
{
	P_GET_PROPERTY(FDoubleProperty,Z_Param_deltaTime);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(double*)Z_Param__Result=UCommonToolboxBPLibrary::GetFPS(Z_Param_deltaTime);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function GetFPS

// Begin Class UCommonToolboxBPLibrary Function GetKineticEnergy
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics
{
	struct CommonToolboxBPLibrary_eventGetKineticEnergy_Parms
	{
		FVector velocity;
		float mass;
		double distanceTraveled;
		FVector ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Physic" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Get the Kinetic Energy of a body of mass (Kg), traveling at velocity. Set distance travelled to get the force.\n" },
#endif
		{ "CPP_Default_distanceTraveled", "1.000000" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Get the Kinetic Energy of a body of mass (Kg), traveling at velocity. Set distance travelled to get the force." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_velocity_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_mass_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_distanceTraveled_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_velocity;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_mass;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_distanceTraveled;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::NewProp_velocity = { "velocity", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetKineticEnergy_Parms, velocity), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_velocity_MetaData), NewProp_velocity_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::NewProp_mass = { "mass", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetKineticEnergy_Parms, mass), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_mass_MetaData), NewProp_mass_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::NewProp_distanceTraveled = { "distanceTraveled", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetKineticEnergy_Parms, distanceTraveled), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_distanceTraveled_MetaData), NewProp_distanceTraveled_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetKineticEnergy_Parms, ReturnValue), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::NewProp_velocity,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::NewProp_mass,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::NewProp_distanceTraveled,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "GetKineticEnergy", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::CommonToolboxBPLibrary_eventGetKineticEnergy_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04822401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::CommonToolboxBPLibrary_eventGetKineticEnergy_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execGetKineticEnergy)
{
	P_GET_STRUCT(FVector,Z_Param_velocity);
	P_GET_PROPERTY(FFloatProperty,Z_Param_mass);
	P_GET_PROPERTY(FDoubleProperty,Z_Param_distanceTraveled);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FVector*)Z_Param__Result=UCommonToolboxBPLibrary::GetKineticEnergy(Z_Param_velocity,Z_Param_mass,Z_Param_distanceTraveled);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function GetKineticEnergy

// Begin Class UCommonToolboxBPLibrary Function GetMontageCurrentWeight
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics
{
	struct CommonToolboxBPLibrary_eventGetMontageCurrentWeight_Parms
	{
		const UAnimInstance* AnimInstance;
		const UAnimMontage* Montage;
		double ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Animation" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Get the weight of the montage specified or of the active montage if NULL Montage.\n" },
#endif
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Get the weight of the montage specified or of the active montage if NULL Montage." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AnimInstance_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Montage_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AnimInstance;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Montage;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::NewProp_AnimInstance = { "AnimInstance", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetMontageCurrentWeight_Parms, AnimInstance), Z_Construct_UClass_UAnimInstance_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AnimInstance_MetaData), NewProp_AnimInstance_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::NewProp_Montage = { "Montage", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetMontageCurrentWeight_Parms, Montage), Z_Construct_UClass_UAnimMontage_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Montage_MetaData), NewProp_Montage_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetMontageCurrentWeight_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::NewProp_AnimInstance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::NewProp_Montage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "GetMontageCurrentWeight", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::CommonToolboxBPLibrary_eventGetMontageCurrentWeight_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::CommonToolboxBPLibrary_eventGetMontageCurrentWeight_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execGetMontageCurrentWeight)
{
	P_GET_OBJECT(UAnimInstance,Z_Param_AnimInstance);
	P_GET_OBJECT(UAnimMontage,Z_Param_Montage);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(double*)Z_Param__Result=UCommonToolboxBPLibrary::GetMontageCurrentWeight(Z_Param_AnimInstance,Z_Param_Montage);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function GetMontageCurrentWeight

// Begin Class UCommonToolboxBPLibrary Function GetSnapOnSurfaceVector
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics
{
	struct CommonToolboxBPLibrary_eventGetSnapOnSurfaceVector_Parms
	{
		FVector point;
		FHitResult surface;
		FVector axis;
		FVector ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Vectors" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "//Get the vector translating point to surface impact point along an axis if provided\n" },
#endif
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Get the vector translating point to surface impact point along an axis if provided" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_point_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_surface_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_axis_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_point;
	static const UECodeGen_Private::FStructPropertyParams NewProp_surface;
	static const UECodeGen_Private::FStructPropertyParams NewProp_axis;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::NewProp_point = { "point", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetSnapOnSurfaceVector_Parms, point), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_point_MetaData), NewProp_point_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::NewProp_surface = { "surface", nullptr, (EPropertyFlags)0x0010008000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetSnapOnSurfaceVector_Parms, surface), Z_Construct_UScriptStruct_FHitResult, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_surface_MetaData), NewProp_surface_MetaData) }; // 4100991306
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::NewProp_axis = { "axis", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetSnapOnSurfaceVector_Parms, axis), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_axis_MetaData), NewProp_axis_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventGetSnapOnSurfaceVector_Parms, ReturnValue), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::NewProp_point,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::NewProp_surface,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::NewProp_axis,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "GetSnapOnSurfaceVector", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::CommonToolboxBPLibrary_eventGetSnapOnSurfaceVector_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04822401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::CommonToolboxBPLibrary_eventGetSnapOnSurfaceVector_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execGetSnapOnSurfaceVector)
{
	P_GET_STRUCT(FVector,Z_Param_point);
	P_GET_STRUCT(FHitResult,Z_Param_surface);
	P_GET_STRUCT(FVector,Z_Param_axis);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FVector*)Z_Param__Result=UCommonToolboxBPLibrary::GetSnapOnSurfaceVector(Z_Param_point,Z_Param_surface,Z_Param_axis);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function GetSnapOnSurfaceVector

// Begin Class UCommonToolboxBPLibrary Function IndexesToBoolArray
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics
{
	struct CommonToolboxBPLibrary_eventIndexesToBoolArray_Parms
	{
		TArray<int32> array;
		TArray<bool> ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Conversions" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Convert an int array of indexes to an bool array\n" },
#endif
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Convert an int array of indexes to an bool array" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_array_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_array_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_array;
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::NewProp_array_Inner = { "array", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::NewProp_array = { "array", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventIndexesToBoolArray_Parms, array), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_array_MetaData), NewProp_array_MetaData) };
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::NewProp_ReturnValue_Inner = { "ReturnValue", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventIndexesToBoolArray_Parms, ReturnValue), EArrayPropertyFlags::None, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::NewProp_array_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::NewProp_array,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::NewProp_ReturnValue_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "IndexesToBoolArray", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::CommonToolboxBPLibrary_eventIndexesToBoolArray_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::CommonToolboxBPLibrary_eventIndexesToBoolArray_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execIndexesToBoolArray)
{
	P_GET_TARRAY(int32,Z_Param_array);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(TArray<bool>*)Z_Param__Result=UCommonToolboxBPLibrary::IndexesToBoolArray(Z_Param_array);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function IndexesToBoolArray

// Begin Class UCommonToolboxBPLibrary Function IndexToFlag
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics
{
	struct CommonToolboxBPLibrary_eventIndexToFlag_Parms
	{
		int32 index;
		int32 ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Conversions" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Convert an index to integer flag.\n" },
#endif
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Convert an index to integer flag." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_index_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_index;
	static const UECodeGen_Private::FIntPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::NewProp_index = { "index", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventIndexToFlag_Parms, index), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_index_MetaData), NewProp_index_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventIndexToFlag_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::NewProp_index,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "IndexToFlag", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::CommonToolboxBPLibrary_eventIndexToFlag_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::CommonToolboxBPLibrary_eventIndexToFlag_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execIndexToFlag)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_index);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(int32*)Z_Param__Result=UCommonToolboxBPLibrary::IndexToFlag(Z_Param_index);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function IndexToFlag

// Begin Class UCommonToolboxBPLibrary Function IsVectorCone
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics
{
	struct CommonToolboxBPLibrary_eventIsVectorCone_Parms
	{
		FVector inVector;
		FVector normal;
		float alphaAngle;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Vectors" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Check if a vector is inside a cone defined by normal and angle in degrees.\n" },
#endif
		{ "CPP_Default_alphaAngle", "90.000000" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Check if a vector is inside a cone defined by normal and angle in degrees." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_inVector_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_normal_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_alphaAngle_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_inVector;
	static const UECodeGen_Private::FStructPropertyParams NewProp_normal;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_alphaAngle;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_inVector = { "inVector", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventIsVectorCone_Parms, inVector), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_inVector_MetaData), NewProp_inVector_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_normal = { "normal", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventIsVectorCone_Parms, normal), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_normal_MetaData), NewProp_normal_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_alphaAngle = { "alphaAngle", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventIsVectorCone_Parms, alphaAngle), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_alphaAngle_MetaData), NewProp_alphaAngle_MetaData) };
void Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((CommonToolboxBPLibrary_eventIsVectorCone_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CommonToolboxBPLibrary_eventIsVectorCone_Parms), &Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_inVector,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_normal,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_alphaAngle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "IsVectorCone", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::CommonToolboxBPLibrary_eventIsVectorCone_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14822401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::CommonToolboxBPLibrary_eventIsVectorCone_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execIsVectorCone)
{
	P_GET_STRUCT(FVector,Z_Param_inVector);
	P_GET_STRUCT(FVector,Z_Param_normal);
	P_GET_PROPERTY(FFloatProperty,Z_Param_alphaAngle);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=UCommonToolboxBPLibrary::IsVectorCone(Z_Param_inVector,Z_Param_normal,Z_Param_alphaAngle);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function IsVectorCone

// Begin Class UCommonToolboxBPLibrary Function Project2DInputs
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics
{
	struct CommonToolboxBPLibrary_eventProject2DInputs_Parms
	{
		FVector2D input;
		FTransform transformRelative;
		FVector planeNormal;
		FVector ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "AdvancedDisplay", "2" },
		{ "Category", "Common Toolbox | Vectors" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Project a 2D axis into 3D space using a transform. (transform.Fwd * input.Y & transform.Right * input.X)\n" },
#endif
		{ "CPP_Default_planeNormal", "0.000000,0.000000,0.000000" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Project a 2D axis into 3D space using a transform. (transform.Fwd * input.Y & transform.Right * input.X)" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_input;
	static const UECodeGen_Private::FStructPropertyParams NewProp_transformRelative;
	static const UECodeGen_Private::FStructPropertyParams NewProp_planeNormal;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::NewProp_input = { "input", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventProject2DInputs_Parms, input), Z_Construct_UScriptStruct_FVector2D, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::NewProp_transformRelative = { "transformRelative", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventProject2DInputs_Parms, transformRelative), Z_Construct_UScriptStruct_FTransform, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::NewProp_planeNormal = { "planeNormal", nullptr, (EPropertyFlags)0x0010040000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventProject2DInputs_Parms, planeNormal), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventProject2DInputs_Parms, ReturnValue), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::NewProp_input,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::NewProp_transformRelative,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::NewProp_planeNormal,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "Project2DInputs", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::CommonToolboxBPLibrary_eventProject2DInputs_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04822401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::CommonToolboxBPLibrary_eventProject2DInputs_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execProject2DInputs)
{
	P_GET_STRUCT(FVector2D,Z_Param_input);
	P_GET_STRUCT(FTransform,Z_Param_transformRelative);
	P_GET_STRUCT(FVector,Z_Param_planeNormal);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FVector*)Z_Param__Result=UCommonToolboxBPLibrary::Project2DInputs(Z_Param_input,Z_Param_transformRelative,Z_Param_planeNormal);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function Project2DInputs

// Begin Class UCommonToolboxBPLibrary Function TenPowX
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics
{
	struct CommonToolboxBPLibrary_eventTenPowX_Parms
	{
		int32 exponent;
		double ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Math" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Returns a power of ten for positive values only.\n" },
#endif
		{ "CompactNodeTitle", "10powX" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Returns a power of ten for positive values only." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_exponent_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_exponent;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::NewProp_exponent = { "exponent", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventTenPowX_Parms, exponent), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_exponent_MetaData), NewProp_exponent_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventTenPowX_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::NewProp_exponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "TenPowX", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::CommonToolboxBPLibrary_eventTenPowX_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::CommonToolboxBPLibrary_eventTenPowX_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execTenPowX)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_exponent);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(double*)Z_Param__Result=UCommonToolboxBPLibrary::TenPowX(Z_Param_exponent);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function TenPowX

// Begin Class UCommonToolboxBPLibrary Function TwoPowX
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics
{
	struct CommonToolboxBPLibrary_eventTwoPowX_Parms
	{
		int32 exponent;
		double ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Math" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Returns a power of two for positive values only.\n" },
#endif
		{ "CompactNodeTitle", "2powX" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Returns a power of two for positive values only." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_exponent_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_exponent;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::NewProp_exponent = { "exponent", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventTwoPowX_Parms, exponent), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_exponent_MetaData), NewProp_exponent_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventTwoPowX_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::NewProp_exponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "TwoPowX", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::CommonToolboxBPLibrary_eventTwoPowX_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::CommonToolboxBPLibrary_eventTwoPowX_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execTwoPowX)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_exponent);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(double*)Z_Param__Result=UCommonToolboxBPLibrary::TwoPowX(Z_Param_exponent);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function TwoPowX

// Begin Class UCommonToolboxBPLibrary Function VectorCone
struct Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics
{
	struct CommonToolboxBPLibrary_eventVectorCone_Parms
	{
		FVector inVector;
		FVector normal;
		float alphaAngle;
		FVector ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Common Toolbox | Vectors" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Restrict a vector inside a cone defined by normal and angle in degrees.\n" },
#endif
		{ "CPP_Default_alphaAngle", "90.000000" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Restrict a vector inside a cone defined by normal and angle in degrees." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_inVector_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_normal_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_alphaAngle_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_inVector;
	static const UECodeGen_Private::FStructPropertyParams NewProp_normal;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_alphaAngle;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::NewProp_inVector = { "inVector", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventVectorCone_Parms, inVector), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_inVector_MetaData), NewProp_inVector_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::NewProp_normal = { "normal", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventVectorCone_Parms, normal), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_normal_MetaData), NewProp_normal_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::NewProp_alphaAngle = { "alphaAngle", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventVectorCone_Parms, alphaAngle), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_alphaAngle_MetaData), NewProp_alphaAngle_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CommonToolboxBPLibrary_eventVectorCone_Parms, ReturnValue), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::NewProp_inVector,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::NewProp_normal,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::NewProp_alphaAngle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCommonToolboxBPLibrary, nullptr, "VectorCone", nullptr, nullptr, Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::CommonToolboxBPLibrary_eventVectorCone_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14822401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::CommonToolboxBPLibrary_eventVectorCone_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCommonToolboxBPLibrary::execVectorCone)
{
	P_GET_STRUCT(FVector,Z_Param_inVector);
	P_GET_STRUCT(FVector,Z_Param_normal);
	P_GET_PROPERTY(FFloatProperty,Z_Param_alphaAngle);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FVector*)Z_Param__Result=UCommonToolboxBPLibrary::VectorCone(Z_Param_inVector,Z_Param_normal,Z_Param_alphaAngle);
	P_NATIVE_END;
}
// End Class UCommonToolboxBPLibrary Function VectorCone

// Begin Class UCommonToolboxBPLibrary
void UCommonToolboxBPLibrary::StaticRegisterNativesUCommonToolboxBPLibrary()
{
	UClass* Class = UCommonToolboxBPLibrary::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "BoolArrayToFlag", &UCommonToolboxBPLibrary::execBoolArrayToFlag },
		{ "BoolToIndexesArray", &UCommonToolboxBPLibrary::execBoolToIndexesArray },
		{ "CheckInRange", &UCommonToolboxBPLibrary::execCheckInRange },
		{ "DrawDebugCircleOnHit", &UCommonToolboxBPLibrary::execDrawDebugCircleOnHit },
		{ "FlagToBoolArray", &UCommonToolboxBPLibrary::execFlagToBoolArray },
		{ "GetFPS", &UCommonToolboxBPLibrary::execGetFPS },
		{ "GetKineticEnergy", &UCommonToolboxBPLibrary::execGetKineticEnergy },
		{ "GetMontageCurrentWeight", &UCommonToolboxBPLibrary::execGetMontageCurrentWeight },
		{ "GetSnapOnSurfaceVector", &UCommonToolboxBPLibrary::execGetSnapOnSurfaceVector },
		{ "IndexesToBoolArray", &UCommonToolboxBPLibrary::execIndexesToBoolArray },
		{ "IndexToFlag", &UCommonToolboxBPLibrary::execIndexToFlag },
		{ "IsVectorCone", &UCommonToolboxBPLibrary::execIsVectorCone },
		{ "Project2DInputs", &UCommonToolboxBPLibrary::execProject2DInputs },
		{ "TenPowX", &UCommonToolboxBPLibrary::execTenPowX },
		{ "TwoPowX", &UCommonToolboxBPLibrary::execTwoPowX },
		{ "VectorCone", &UCommonToolboxBPLibrary::execVectorCone },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCommonToolboxBPLibrary);
UClass* Z_Construct_UClass_UCommonToolboxBPLibrary_NoRegister()
{
	return UCommonToolboxBPLibrary::StaticClass();
}
struct Z_Construct_UClass_UCommonToolboxBPLibrary_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/* \n*\x09""Function library class.\n*\x09""Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.\n*\n*\x09When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.\n*\x09""BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.\n*\x09""BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.\n*\x09""DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.\n*\x09\x09\x09\x09Its lets you name the node using characters not allowed in C++ function names.\n*\x09""CompactNodeTitle - the word(s) that appear on the node.\n*\x09Keywords -\x09the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. \n*\x09\x09\x09\x09Good example is \"Print String\" node which you can find also by using keyword \"log\".\n*\x09""Category -\x09the category your node will be under in the Blueprint drop-down menu.\n*\n*\x09""For more info on custom blueprint nodes visit documentation:\n*\x09https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation\n*/" },
#endif
		{ "IncludePath", "CommonToolboxBPLibrary.h" },
		{ "ModuleRelativePath", "Public/CommonToolboxBPLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "*      Function library class.\n*      Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.\n*\n*      When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.\n*      BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.\n*      BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.\n*      DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.\n*                              Its lets you name the node using characters not allowed in C++ function names.\n*      CompactNodeTitle - the word(s) that appear on the node.\n*      Keywords -      the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu.\n*                              Good example is \"Print String\" node which you can find also by using keyword \"log\".\n*      Category -      the category your node will be under in the Blueprint drop-down menu.\n*\n*      For more info on custom blueprint nodes visit documentation:\n*      https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation" },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolArrayToFlag, "BoolArrayToFlag" }, // 3548769121
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_BoolToIndexesArray, "BoolToIndexesArray" }, // 1859035500
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_CheckInRange, "CheckInRange" }, // 2622474077
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_DrawDebugCircleOnHit, "DrawDebugCircleOnHit" }, // 898777316
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_FlagToBoolArray, "FlagToBoolArray" }, // 3518976633
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_GetFPS, "GetFPS" }, // 1372302845
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_GetKineticEnergy, "GetKineticEnergy" }, // 2528630869
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_GetMontageCurrentWeight, "GetMontageCurrentWeight" }, // 3135465456
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_GetSnapOnSurfaceVector, "GetSnapOnSurfaceVector" }, // 2616424988
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexesToBoolArray, "IndexesToBoolArray" }, // 326601134
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_IndexToFlag, "IndexToFlag" }, // 396229726
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_IsVectorCone, "IsVectorCone" }, // 3514531397
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_Project2DInputs, "Project2DInputs" }, // 738423524
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_TenPowX, "TenPowX" }, // 3173850436
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_TwoPowX, "TwoPowX" }, // 776020332
		{ &Z_Construct_UFunction_UCommonToolboxBPLibrary_VectorCone, "VectorCone" }, // 685258197
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCommonToolboxBPLibrary>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UCommonToolboxBPLibrary_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBlueprintFunctionLibrary,
	(UObject* (*)())Z_Construct_UPackage__Script_CommonToolbox,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCommonToolboxBPLibrary_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCommonToolboxBPLibrary_Statics::ClassParams = {
	&UCommonToolboxBPLibrary::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	0,
	0,
	0x000000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCommonToolboxBPLibrary_Statics::Class_MetaDataParams), Z_Construct_UClass_UCommonToolboxBPLibrary_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCommonToolboxBPLibrary()
{
	if (!Z_Registration_Info_UClass_UCommonToolboxBPLibrary.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCommonToolboxBPLibrary.OuterSingleton, Z_Construct_UClass_UCommonToolboxBPLibrary_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCommonToolboxBPLibrary.OuterSingleton;
}
template<> COMMONTOOLBOX_API UClass* StaticClass<UCommonToolboxBPLibrary>()
{
	return UCommonToolboxBPLibrary::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCommonToolboxBPLibrary);
UCommonToolboxBPLibrary::~UCommonToolboxBPLibrary() {}
// End Class UCommonToolboxBPLibrary

// Begin Registration
struct Z_CompiledInDeferFile_FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCommonToolboxBPLibrary, UCommonToolboxBPLibrary::StaticClass, TEXT("UCommonToolboxBPLibrary"), &Z_Registration_Info_UClass_UCommonToolboxBPLibrary, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCommonToolboxBPLibrary), 3420617647U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_344093333(TEXT("/Script/CommonToolbox"),
	Z_CompiledInDeferFile_FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
