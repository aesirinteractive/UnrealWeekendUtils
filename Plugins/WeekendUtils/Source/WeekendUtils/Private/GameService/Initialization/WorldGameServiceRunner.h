﻿///////////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2023 by Benjamin Barz in cooperation with Nine Worlds Studios GmbH.
///
/// This file is part of the WeekendUtils UE5 Plugin.
///
/// Distributed under the MIT License. See accompanying file LICENSE.
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "WorldGameServiceRunner.generated.h"

/**
 * The singleton instance of this subsystem for each world will take care of maintaining the
 * game service world in cooperation with the persistent @UGameServiceManager.
 *
 * The runner will do the following things:
 * - Register @UGameModeServiceConfigBase objects that apply to the current world
 * - Make sure all @UWorldSubsystem dependencies of configured game services are available
 * - Start all configured game services for the current world in the correct order
 * - Tick all running game services (that want to be ticked)
 * - Shutdown all running services when the world tears down
 * - Clears all registered service configs when the world tears down
 */
UCLASS()
class UWorldGameServiceRunner : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// - UTickableWorldSubsystem
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual void Deinitialize() override;
	// --

private:
	void RegisterAutoServiceConfig();
	void StartRegisteredServices();
	static void TickRunningServices(float DeltaTime);
	static TArray<TSubclassOf<UWorldSubsystem>> GatherWorldSubsystemDependencies();
};
