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
#include "GameService/GameServiceUtils.h"
#include "UObject/WeakInterfacePtr.h"

class UGameServiceBase;
class USubsystem;

/**
 * Grants classes inheriting from FGameServiceUser utilities to configure GameService and Subsystem dependencies
 * and the possibility to directly access those dependencies in their runtime code.
 *
 * Intended usage:
 * - Inherit your @UClass from @FGameServiceUser
 * - Configure dependencies in your class constructor:
 *     @example: ServiceDependencies.Add<USomeGameService>();
 *     @example: SubsystemDependencies.Add<USomeSubsystem>();
 * - Access services directly in your runtime code (requires a fully created world):
 *     @example: USomeGameService& MyDependency = UseGameService<USomeGameService>(this);
 *     @example: TObjectPtr<USomeGameService> MyDependency = UseGameServiceAsPtr<USomeGameService>(this);
 *     @example: TWeakObjectPtr<USomeGameService> MyDependency = UseGameServiceAsWeakPtr<USomeGameService>(this);
 *     @example: TWeakObjectPtr<USomeOtherService> NiceToHaveService = FindOptionalGameService<USomeOtherService>();
 *     @note: Interfaced services use the interface version of storage pointers instead: @TScriptInterface, @TWeakInterfacePtr
 * - Access subsystem dependencies in your runtime code:
 *     @example: TWeakObjectPtr<USomeSubsystem> PotentiallyUnavailableSubsystem = FindSubsystemDependency<USomeSubsystem>(this);
 * - When relying on subsystems or async services whose initialization order might overlap with that of your class, use this:
 *     @example: WaitForDependencies(this, FOnWaitingFinished::CreateUObject(this, &ThisClass::ExecuteCodeRelyingOnAsyncDependencies));
 *
 * @remark Most functionalities require passing a ServiceUser @UObject ('this') in order to access UE framework functionalities.
 */
class WEEKENDUTILS_API FGameServiceUser
{
public:
	using FGameServiceClass = TSubclassOf<UObject>; // => see GameServiceBase.h

	/** @returns all game service classes that this service user depends on. */
	const TArray<FGameServiceClass>& GetServiceClassDependencies() const;

	/** @returns all subsystem classes that this service user depends on. */
	const TArray<TSubclassOf<USubsystem>>& GetSubsystemClassDependencies() const;

	/** @returns whether all game service dependencies are running and all subsystem dependencies are available. */
	bool AreAllDependenciesReady(const UObject* ServiceUser) const;
	bool AreServiceDependenciesReady() const;
	bool AreSubsystemDependenciesReady(const UObject* ServiceUser) const;

protected:
	FGameServiceUser() = default;

	/**
	 * Dependency config container for game services. Call ServiceDependencies.Add<T>() in the constructor of inherited classes.
	 * @remark Supported game service types: @UGameServiceBase, @UInterface
	 */
	FGameServiceDependencies ServiceDependencies;

	/**
	 * Dependency config container for subsystem dependencies. Call SubsystemDependencies.Add<T>() in the constructor of inherited classes.
	 * @remark Supported subsystem types: @UWorldSubsytem, @UEngineSubsytem, @UGameInstanceSubsystem, @LocalPlayerSubsystem
	 */
	FSubsystemDependencies SubsystemDependencies;

	/**
	 * Defers given callback until all dependencies are ready to be used. Triggers callback immediately if that is already the case.
	 * This is only necessary when depending on subsystems whose birth might be after the birth of the service user.
	 * Dependencies to other game services are always available, because they are created + started on demand when not yet running.
	 * However, async services might take a while until they are considered 'running', even when they are started on demand.
	 */
	DECLARE_DELEGATE(FOnWaitingFinished)
	void WaitForDependencies(const UObject* ServiceUser, FOnWaitingFinished Callback);
	void WaitForDependencies(const UObject* ServiceUser, TFunction<void( )> Callback);

	/**
	 * @returns another game service that is part of the ServiceDependencies config.
	 * If the desired service is not yet created or running, it will be created and started on demand.
	 */
	UGameServiceBase& UseGameService(const UObject* ServiceUser, const FGameServiceClass& ServiceClass) const;

	/** @returns another game service as weak pointer, even when not configured as dependency, considering it might not be available. */
	TWeakObjectPtr<UGameServiceBase> FindOptionalGameService(const FGameServiceClass& ServiceClass) const;

	/** @returns a subsystem that was configured as dependency as weak pointer, considering it might not yet be available. */
	TWeakObjectPtr<USubsystem> FindSubsystemDependency(const UObject* ServiceUser, const TSubclassOf<USubsystem>& SubsystemClass) const;

	/** @returns whether a certain game service class is currently registered. */
	bool IsGameServiceRegistered(const FGameServiceClass& ServiceClass) const;

	template<typename T> typename TEnableIf<TIsDerivedFrom<T, UGameServiceBase>::IsDerived, T&>::Type
	/*(T&)*/ UseGameService(const UObject* ServiceUser)
	{
		return *Cast<T>(&UseGameService(ServiceUser, T::StaticClass()));
	}

	template<typename T> typename TEnableIf<TIsDerivedFrom<T, UGameServiceBase>::IsDerived, TObjectPtr<T>>::Type
	/*(TObjectPtr<T>)*/ UseGameServiceAsPtr(const UObject* ServiceUser) const
	{
		return TObjectPtr<T>(Cast<T>(&UseGameService(ServiceUser, T::StaticClass())));
	}

	template<typename T> typename TEnableIf<TIsIInterface<T>::Value, TScriptInterface<T>>::Type
	/*(TScriptInterface<T>)*/ UseGameServiceAsPtr(const UObject* ServiceUser) const
	{
		return TScriptInterface<T>(&UseGameService(ServiceUser, T::UClassType::StaticClass()));
	}

	template<typename T> typename TEnableIf<TIsDerivedFrom<T, UGameServiceBase>::IsDerived, TWeakObjectPtr<T>>::Type
	/*(TWeakObjectPtr<T>)*/ UseGameServiceAsWeakPtr(const UObject* ServiceUser) const
	{
		return TWeakObjectPtr<T>(Cast<T>(&UseGameService(ServiceUser, T::StaticClass())));
	}

	template<typename T> typename TEnableIf<TIsIInterface<T>::Value, TWeakInterfacePtr<T>>::Type
	/*(TWeakInterfacePtr<T>)*/ UseGameServiceAsWeakPtr(const UObject* ServiceUser) const
	{
		return TWeakInterfacePtr<T>(&UseGameService(ServiceUser, T::UClassType::StaticClass()));
	}

	template<typename T> typename TEnableIf<TIsDerivedFrom<T, UGameServiceBase>::IsDerived, TWeakObjectPtr<T>>::Type
	/*(TWeakObjectPtr<T>)*/ FindOptionalGameService() const
	{
		return TWeakObjectPtr<T>(Cast<T>(FindOptionalGameService(T::StaticClass()).Get()));
	}

	template<typename T> typename TEnableIf<TIsIInterface<T>::Value, TWeakInterfacePtr<T>>::Type
	/*(TWeakInterfacePtr<T>)*/ FindOptionalGameService() const
	{
		return TWeakInterfacePtr<T>(FindOptionalGameService(T::UClassType::StaticClass()).Get());
	}

	template<typename T>
	TWeakObjectPtr<T> FindSubsystemDependency(const UObject* ServiceUser) const
	{
		static_assert(TIsDerivedFrom<T, USubsystem>::IsDerived);
		return TWeakObjectPtr<T>(Cast<T>(FindSubsystemDependency(ServiceUser, T::StaticClass()).Get()));
	}

	template<typename T>
	bool IsGameServiceRegistered() const
	{
		return IsGameServiceRegistered(GameService::GetServiceUClass<T>());
	}

	/** When waiting for dependencies, this is called automatically each tick, but can also called manually by derived class. */
	void PollPendingDependencyWaitCallbacks(const UObject* ServiceUser);

	/** When waiting for dependencies, this can be called when the wait should be cancelled, i.e. when the service user is prematurely destroyed. */
	void StopWaitingForDependencies(const UObject* ServiceUser);

private:
	TArray<FOnWaitingFinished> PendingDependencyWaitCallbacks;
};
