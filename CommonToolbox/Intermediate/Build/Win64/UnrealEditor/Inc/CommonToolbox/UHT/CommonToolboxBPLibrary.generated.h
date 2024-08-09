// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "CommonToolboxBPLibrary.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UAnimInstance;
class UAnimMontage;
struct FHitResult;
struct FLinearColor;
#ifdef COMMONTOOLBOX_CommonToolboxBPLibrary_generated_h
#error "CommonToolboxBPLibrary.generated.h already included, missing '#pragma once' in CommonToolboxBPLibrary.h"
#endif
#define COMMONTOOLBOX_CommonToolboxBPLibrary_generated_h

#define FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_31_RPC_WRAPPERS \
	DECLARE_FUNCTION(execDrawDebugCircleOnHit); \
	DECLARE_FUNCTION(execGetKineticEnergy); \
	DECLARE_FUNCTION(execGetMontageCurrentWeight); \
	DECLARE_FUNCTION(execGetSnapOnSurfaceVector); \
	DECLARE_FUNCTION(execProject2DInputs); \
	DECLARE_FUNCTION(execIsVectorCone); \
	DECLARE_FUNCTION(execVectorCone); \
	DECLARE_FUNCTION(execIndexToFlag); \
	DECLARE_FUNCTION(execIndexesToBoolArray); \
	DECLARE_FUNCTION(execBoolToIndexesArray); \
	DECLARE_FUNCTION(execFlagToBoolArray); \
	DECLARE_FUNCTION(execBoolArrayToFlag); \
	DECLARE_FUNCTION(execGetFPS); \
	DECLARE_FUNCTION(execTwoPowX); \
	DECLARE_FUNCTION(execTenPowX); \
	DECLARE_FUNCTION(execCheckInRange);


#define FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_31_INCLASS \
private: \
	static void StaticRegisterNativesUCommonToolboxBPLibrary(); \
	friend struct Z_Construct_UClass_UCommonToolboxBPLibrary_Statics; \
public: \
	DECLARE_CLASS(UCommonToolboxBPLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/CommonToolbox"), NO_API) \
	DECLARE_SERIALIZER(UCommonToolboxBPLibrary)


#define FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_31_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UCommonToolboxBPLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UCommonToolboxBPLibrary) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UCommonToolboxBPLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UCommonToolboxBPLibrary); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	UCommonToolboxBPLibrary(UCommonToolboxBPLibrary&&); \
	UCommonToolboxBPLibrary(const UCommonToolboxBPLibrary&); \
public: \
	NO_API virtual ~UCommonToolboxBPLibrary();


#define FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_28_PROLOG
#define FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_31_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_31_RPC_WRAPPERS \
	FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_31_INCLASS \
	FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h_31_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> COMMONTOOLBOX_API UClass* StaticClass<class UCommonToolboxBPLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_RandD_UnrealProjects_ModularController_Plugins_CommonToolbox_Source_CommonToolbox_Public_CommonToolboxBPLibrary_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
