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
#include "GameService/GameServiceBase.h"

#include "AsyncGameServiceBase.generated.h"

DECLARE_DELEGATE(FOnAsyncGameServiceStarted)

/**
 * Extension base class of @UGameServiceBase that provides additional utilities and interface
 * to start and shutdown a derived service in a deferred way.
 */
UCLASS(Abstract)
class WEEKENDUTILS_API UAsyncGameServiceBase : public UGameServiceBase
{
	GENERATED_BODY()

public:
	// - UGameServiceBase
	virtual void StartService() override final;
	virtual bool IsTickable() const override { return IsServiceRunning(); }
	virtual void ShutdownService() override;
	// --

	/** @returns whether the service has finished starting and is currently considered 'running'. */
	virtual bool IsServiceRunning() const { return (CurrentStatus == EAsyncServiceStatus::Running); }

	/** Calls provided callback right after this service is fully started and running - or immediately if already running. */
	void WaitUntilServiceIsRunning(FOnAsyncGameServiceStarted Callback);

	/**
	 * Called when the service starts to kick off the deferred starting process.
	 * Derived classes must manually call @FinishServiceStart() when they are fully started.
	 * when the service is considered 'running'.
	 */
	virtual void BeginServiceStart() PURE_VIRTUAL(BeginServiceStart);
	void FinishServiceStart();

	/**
	 * Called when the service starts to kick off the deferred shutdown process.
	 * Derived classes must manually call @FinishServiceShutdown() when they are fully shut down.
	 * Be aware that any service dependencies are only promised to be valid in the BeginServiceShutdown() call.
	 * The service object will be artificially kept alive until it is fully shutdown, except for when
	 * @bIsWorldTearingDown is true, then the service should shutdown immediately (if possible).
	 */
	virtual void BeginServiceShutdown(bool bIsWorldTearingDown) PURE_VIRTUAL(BeginServiceShutdown);
	void FinishServiceShutdown();

protected:
	enum class EAsyncServiceStatus : uint8
	{
		Inactive = 0,
		Starting = 1,
		Running = 2,
		Stopping = 3,
	} CurrentStatus = EAsyncServiceStatus::Inactive;

	/** When enabled by derived class, the @BeginServiceStart() call will be deferred until all configured dependencies are available. */
	bool bWaitForDependenciesBeforeStarting = true;

private:
	TArray<FOnAsyncGameServiceStarted> PendingServiceStartCallbacks;
};
