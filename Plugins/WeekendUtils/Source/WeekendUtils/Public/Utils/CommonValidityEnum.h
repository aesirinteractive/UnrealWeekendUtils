﻿///////////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2023 by Benjamin Barz and contributors. See file: CREDITS.md
///
/// This file is part of the WeekendUtils UE5 Plugin.
///
/// Distributed under the MIT License. See file: LICENSE.md
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "EnumUtils.h"

///////////////////////////////////////////////////////////////////////////////////////
/// UFUNCTION(BlueprintCallable, BlueprintPure = False, ExpandEnumAsExecs = "ReturnValue")
/// ECommonValidity BranchValidity() const;

UENUM(BlueprintType)
enum class ECommonValidity : uint8
{
	Valid,
	Invalid
};

DEFINE_ENUM_STRING_CONVERTERS(CommonValidity, ECommonValidity);
DEFINE_ENUM_BOOL_CONVERTERS(CommonValidity, ECommonValidity, Valid, Invalid);

///////////////////////////////////////////////////////////////////////////////////////
