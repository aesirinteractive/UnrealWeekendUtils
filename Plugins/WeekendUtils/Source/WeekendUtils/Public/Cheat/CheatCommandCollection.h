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
#include "CheatMenuSettings.h"

class ICheatCommand;

namespace Cheats
{
	class WEEKENDUTILS_API FCheatCommandCollection
	{
	public:
		FCheatCommandCollection();
		FCheatCommandCollection(const FCheatMenuCategorySettings& InCheatMenuSettings);

		void AddCheat(ICheatCommand* CheatCommand);
		TArray<ICheatCommand*> GetRegisteredCheatCommands() const { return RegisteredCheatCommands; }

		bool ShowInCheatMenu() const { return CheatMenuSettings.IsSet(); }
		FCheatMenuCategorySettings GetCheatMenuSettings() const { return *CheatMenuSettings; }

	private:
		TOptional<FCheatMenuCategorySettings> CheatMenuSettings;
		TArray<ICheatCommand*> RegisteredCheatCommands;
	};

	WEEKENDUTILS_API TArray<FCheatCommandCollection*>& GetAllCollections();

	/** Utility for #DEFINE_CHEAT_COLLECTION macro. @see CheatCommand.h */
	inline FCheatMenuCategorySettings AsCheatMenuTab(FName TabName) { return FCheatMenuCategorySettings().Tab(TabName); }
}
