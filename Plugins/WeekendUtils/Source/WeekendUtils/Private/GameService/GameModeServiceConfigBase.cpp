﻿///////////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2023 by Benjamin Barz in cooperation with Nine Worlds Studios GmbH.
///
/// This file is part of the WeekendUtils UE5 Plugin.
///
/// Distributed under the MIT License. See accompanying file LICENSE.
///
///////////////////////////////////////////////////////////////////////////////////////

#include "GameService/GameModeServiceConfigBase.h"

namespace Internal
{
	static TMap<TSubclassOf<AGameModeBase>, TSubclassOf<UGameModeServiceConfigBase>> GConfigClassesByGameModes;
}

void UGameModeServiceConfigBase::RegisterForMapsWithGameMode(const TSubclassOf<AGameModeBase>& GameModeClass)
{
	TSubclassOf<UGameModeServiceConfigBase>& RegisteredConfigClass = Internal::GConfigClassesByGameModes.FindOrAdd(GameModeClass);
	if (RegisteredConfigClass != nullptr && RegisteredConfigClass != GameModeClass)
	{
		// Only one AutoRegisteredGameServiceConfig per game mode is allowed:
		ensureMsgf(false, TEXT("GameServiceConfig for GameMode %s already has another config class configured!"));
	}

	RegisteredConfigClass = GetClass();
}

bool UGameModeServiceConfigBase::ShouldUseWithGameMode(const TSubclassOf<AGameModeBase>& GameModeClass) const
{
	for (const auto Itr : Internal::GConfigClassesByGameModes)
	{
		const TSubclassOf<AGameModeBase> ConfiguredGameMode = Itr.Key;
		const TSubclassOf<UGameModeServiceConfigBase> ConfiguredConfig = Itr.Value;

		if (ConfiguredGameMode->IsChildOf(GameModeClass))
			return this->IsA(ConfiguredConfig);
	}

	return false;
}

const UGameModeServiceConfigBase* UGameModeServiceConfigBase::FindConfigForWorld(const UWorld& World)
{
	const TSubclassOf<AGameModeBase>& CurrentGameModeClass = World.GetWorldSettings()->DefaultGameMode;
	if (CurrentGameModeClass == nullptr)
		return nullptr; // No game mode configured in world settings.

	for (const auto Itr : Internal::GConfigClassesByGameModes)
	{
		const TSubclassOf<AGameModeBase> ConfiguredGameMode = Itr.Key;
		const TSubclassOf<UGameModeServiceConfigBase> ConfiguredConfig = Itr.Value;

		if (CurrentGameModeClass->IsChildOf(ConfiguredGameMode))
			return ConfiguredConfig->GetDefaultObject<UGameModeServiceConfigBase>();
	}

	return nullptr; // No config found for game mode.
}
