# WeekendGameService

## Motivation

- Subsystems with better control over lifetime, initialization order and dependencies
    - Less validity checks and waiting for initialization
- Mockable (for tests)

## Philosophy

- Services are standalone singleton instances, registered by a service class
- Service users configure their service dependencies via `ConfigureGameServiceUser()`
    - Configured dependencies are automatically resolved
- Services are (in most use-cases) always available and ready to be used by service users

## Limitations

- Inheritance from `UGameServiceBase`
- One service per service class
- Currently not replicated (no multiplayer support)
- Services that `ShutdownWithGameInstance` cannot have dependencies to services with `ShutdownWithWorld` lifetime

# Architecture

## Principles

- `UGameServiceManager` (singleton bound to `UGameInstance`) runs and maintains service instances
    - Access should flow through `FGameServiceUser` or `UGameServiceLocator`, not through `UGameServiceManager`
- Project-specific `UGameServiceConfig` classes register services by register-class (key) and implementation-class (value)
    - Services can be registered by interface (as register-class), but it's rarely necessary since games have not many use-cases for multiple implementation
    - Configs are similar to dependency containers and discretely define which services a game environment needs
- Service users define their dependencies (via register-class, not caring about which implementation-class is behind)
    - Service users should NEVER cast the service to a specific implementation-class
- Blueprints can access services only via `UGameServiceLocator`, but should do so only when really necessary
    - Blueprints should use ViewModels when possible to communicate with services

## Service Lifetime

On first glance, this will look familiar to how Subsystems work:
![WeekendUtils-GameService (1)](https://github.com/barzb/UnrealWeekendUtils/assets/14543036/7ce4278c-6c6f-450c-9f7b-d38938674889)


### Start Service

- All registered services are started by `UWorldGameServiceRunner` (TickableWorldSubsystem), _before_ any actors receive `BeginPlay()`
- Services with dependencies to other services will be started after their dependencies have started
    - `StartService()` is called on each service and should be derived by the service implementation
- Services that were not configured by a `UGameServiceConfig `will be started on demand when declared as dependency by another service or service user
    - It is recommended to always configure all used services by environment via `UGameServiceConfig`
    - Interface-registered services and abstract services cannot be started on demand, because no implementation-class can be determined

### Tick Service

- Services that overwrite `IsTickable()` to be true will be ticked automatically by the `UWorldGameServiceRunner`
    - The result of `IsTickable()` is allowed to change at runtime, it will be re-evaluated each tick

### Shutdown Service

- Services with the "ShutdownWithWorld" lifetime are shut down and destroyed when the world cleans up, _after_ all actors receive `EndPlay()`
    - Service users in the actor framework can safely access service dependencies in their EndPlay to clean up
    - ShutdownService() is called on respective services and should be derived by the service implementation
- Once a new level has been entered, its configured services are started once again and new service instances are created
    - The only exception are services with the "ShutdownWithGameInstance" lifetime, which will keep running uninterrupted
- Once the application closes and the GameInstance shuts down, the remaining services are shut down with it

## Use & Find Game Services

- Classes inheriting `FGameServiceUser` gain access to various utilities to use service dependencies
    - `FGameServiceUser::UseGameService()` will always return the service reference, no valid checks needed. Missing services are started on demand
    - `FGameServiceUser::FindOptionalGameService()` will return a service pointer that may be invalid, should the service not be configured in the current environment
- `UGameServiceLocator::FindService()` behaves similar to `FGameServiceUser::FindOptionalGameService()` without needing to inherit `FGameServiceUser`
    - It is still recommended to inherit `FGameServiceUser` in most cases
    - All `UGameServiceLocator` static methods require a `WorldContext` argument to resolve the correct `UGameServiceManager`

## Async Services

- Services can also inherit from `UAsyncGameServiceBase`, should they depend on subsystems or other externally managed objects or API
- Service users can use `UAsyncGameServiceBase::WaitUntilServiceIsRunning()` or `FGameServiceUser::WaitForDependencies` to receive a callback once the service is running

# Debugging

- GameplayDebuggerCategory_GameServices "GameServices" show the current status of all registered services

# Project Integration

## Game Service Config

- It is recommended to create one game service config per environment, i.e. "MainMenuGameServiceConfig" and "CoreGameServiceConfig"
- Game service configs should be implemented in code by deriving from `UGameModeServiceConfigBase`:
```cpp
// [Config = Game, DefaultConfig] Properties are saved into the DefaultGame.ini
UCLASS(Config = Game, DefaultConfig)
class UCoreGameServiceConfig : public UGameModeServiceConfigBase
{
	GENERATED_BODY()
public:
	UCoreGameServiceConfig()
	{
		// This instanced object acts as a template object for the registered service and passes all
		// "Config" properties to the started service:
		TimeServiceTemplate = CreateDefaultSubobject<UTimeService>("TimeServiceTemplate");
	}

	virtual void Configure() override
	{
		// This config will be used for all worlds with specified game mode class:
		RegisterFor<ACoreGameMode>();

		AddService<USaveGameService>(); // Implementation = RegisterClass
		AddService<IResourceProviderServiceInterface, UResourceProviderService>();
		AddService<UScenarioService>(ScenarioServiceClass);
		AddService<UTimeService>(*TimeServiceTemplate); // Implementation based on Template
	}

protected:
	// This service instance class can be configured in the "Game Services" project settings:
	UPROPERTY(Config, EditAnywhere, NoClear)
	TSubclassOf<UScenarioService> ScenarioServiceClass = UScenarioService::StaticClass();

	// This service instance template can be configured in the "Game Services" project settings:
	UPROPERTY(Instanced, EditAnywhere, NoClear)
	TObjectPtr<UTimeService> TimeServiceTemplate;
};
```

- To configure a service template, the game service class must add the "Config" specifier in UCLASS() and all configurable UPROPERTY() members:
```cpp
// [Config = Game, DefaultConfig] Properties are saved into the DefaultGame.ini
// [PerObjectConfig] Properties are saved per instance, otherwise properties are saved for the CDO
// [Blueprintable] TimeService can be derived by Blueprints
UCLASS(Config = Game, DefaultConfig, PerObjectConfig, Blueprintable)
class UTimeService : public UGameServiceBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(Config, EditDefaultsOnly)
	float TimeScale = 1.f;

	UPROPERTY(Config, EditDefaultsOnly)
	FDateTime CurrentTime = FDateTime(0);
};
```
![](https://github.com/barzb/UnrealWeekendUtils/assets/14543036/ae5f68f5-4946-4bfb-b274-8db1df796007)

## Inheriting FGameServiceUser

- `FGameServiceUser` requires you to do evil multiple inheritance, but view it more like a class extension:
```cpp
// Class declaration:
class UCurrentSaveGameViewModel : public UMVVMViewModelBase, public FGameServiceUser
{
	// ...
	virtual FGameServiceUserConfig ConfigureGameServiceUser() const override;
};

// Class implementation:
FGameServiceUserConfig UCurrentSaveGameViewModel::ConfigureGameServiceUser() const
{
	return FGameServiceUserConfig(this)
		.AddServiceDependency<USaveGameService>();
}

void UCurrentSaveGameViewModel::BeginUsage()
{
	USaveGameService& TmpSaveGameService = UseGameService<USaveGameService>();
	// Access service directly or store it as member:
	SaveGameServiceStoragePtr = UseGameServiceAsPtr<USaveGameService>();
}
```
- Service users can also configure SubsystemDependencies and OptionalSubsystemDependencies, but will have to call `WaitForDependencies()` since subsystems may be created at a later point

## When should something be a game service?

- This depends entirely on your project's architecture, but here are some use-cases that match the philosophy of the framework:
    - Systems that you would otherwise create a `UWorldSubsystem` for
    - Systems that process data of - or make decisions about - the state of the game world, like a scenario director, feature balancing system, ...
    - Systems that you want to mock for tests
- Services are a tool, not a rule. You should always evaluate the benefit from using a service in comparison to an actor or component, or even a subsystem
- Do not misuse the framework for use-cases it was not designed for

## Examples

- Check out the [WeekendUtilsTests](https://github.com/barzb/WeekendUtils/tree/main/Source/WeekendUtilsTests/Private/GameService/Specs) module for implementation and usage examples