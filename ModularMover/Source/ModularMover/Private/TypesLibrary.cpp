// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "TypesLibrary.h"


#pragma region Enums
#pragma endregion

#pragma region Structs

FBaseMoverStateParams::FBaseMoverStateParams()
{
}

bool FBaseMoverStateParams::IsValid()
{
	return StateName != NAME_None && Priority >= 0;
}

#pragma endregion

#pragma region Classes

UMoverState::UMoverState()
{
}

template <typename T>
std::enable_if<std::is_base_of_v<FBaseMoverStateParams, T>, T> UMoverState::GetMoverStateParam_Implementation() const
{
	return FBaseMoverStateParams();
}

#pragma endregion
