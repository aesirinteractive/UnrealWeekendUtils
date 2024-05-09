﻿///////////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) by Benjamin Barz and contributors. See file: CREDITS.md
///
/// This file is part of the WeekendUtils UE5 Plugin.
///
/// Distributed under the MIT License. See file: LICENSE.md
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"

struct WEEKENDUTILS_API FCheatMenuCategorySettings
{
public:
	FCheatMenuCategorySettings& Tab(FName TabName) { MenuTabName = TabName; return *this; }
	FCheatMenuCategorySettings& Section(FName SectionName) { MenuSectionName = SectionName; return *this; }

	TOptional<FName> MenuTabName = {};
	TOptional<FName> MenuSectionName = {};
};

namespace Cheats::EVariableStyle
{
	enum Type
	{
		Number,
		FloatNumber,
		Text,
		TrueFalse,
		DropdownText
	};

	static FString LexToString(const Type& VariableStyle)
	{
		switch (VariableStyle)
		{
			case Number: return "Number";
			case FloatNumber: return "Float Number";
			case Text: return "Text";
			case TrueFalse: return "True/False";
			case DropdownText: return "Dropdown Text";
			default: return "??";
		}
	}

	template <typename T>
	static Type FromType()
	{
		if (std::is_same_v<T, bool>)
			return TrueFalse;
		if (TIsIntegral<T>::Value)
			return Number;
		if (TIsFloatingPoint<T>::Value)
			return FloatNumber;
		return Text;
	}
}
