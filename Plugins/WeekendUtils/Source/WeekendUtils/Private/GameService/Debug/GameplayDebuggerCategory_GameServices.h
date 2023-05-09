﻿///////////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2023 by Benjamin Barz in cooperation with Nine Worlds Studios GmbH.
///
/// This file is part of the WeekendUtils UE5 Plugin.
///
/// Distributed under the MIT License. See accompanying file LICENSE.
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if WITH_GAMEPLAY_DEBUGGER

#include "CoreMinimal.h"
#include "GameService/GameServiceBase.h"
#include "GameplayDebugger/GameplayDebuggerUtils.h"
#include "GameplayDebuggerCategory.h"

class FGameplayDebuggerCategory_GameServices final : public FGameplayDebuggerCategory
{
public:
	FGameplayDebuggerCategory_GameServices();
	GENERATE_DEBUGGER_CATEGORY(GameServices);

	// - FGameplayDebuggerCategory
	virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	// --

private:
	void ToggleShowDependencies();
	bool ShouldShowDependencies() const;

	TArray<FString> CollectServiceDependenciesInfo(const FGameServiceClass& ServiceClass) const;

	static FString BoolToCyanOrOrange(bool bUseCyanOverOrange);
};

#endif // WITH_GAMEPLAY_DEBUGGER