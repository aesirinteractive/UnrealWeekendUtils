# WeekendUtilsTests

## Overview

- Automation test specs that double as living usage examples for the other modules
- `"Type": "UncookedOnly"` in the `.uplugin`, compiled only in editor and non-cooked builds, never shipped
- Depends on all four other modules (`WeekendUtils`, `WeekendGameService`, `WeekendSaveGame`, `WeekendCheatMenu`), it is the top-level integration test module

## Architecture

## Test Framework

- Built on the Unreal Automation Spec framework (`Misc/AutomationTest.h`), wrapped by the `WE_BEGIN_DEFINE_SPEC` / `WE_END_DEFINE_SPEC` macros from `WeekendUtils`
- All specs are guarded by `WITH_AUTOMATION_WORKER`
- Each spec's `BeforeEach` builds a fresh `FScopedAutomationTestWorld`, calls `InitializeGame()`, sets up a `UGameServiceManager` / `UGameServiceConfig` for that test world and instantiates mocks as needed, `AfterEach` tears the world down again

## Spec Files

- `GameService/Specs/GameInstanceServiceTerminator.spec.cpp`
- `GameService/Specs/GameServiceManager.spec.cpp`
- `GameService/Specs/GameServiceUser.spec.cpp`
- `GameService/Specs/WorldGameServiceRunner.spec.cpp`
- `SaveGame/Specs/SaveGameService.spec.cpp`
- `GameServiceUtilsTests.cpp` is a compile-time-only check (`GameService::Tests::FGameServiceUtilsTemplates`) that exercises `TDependencyList<T>` template instantiations, it throws compile errors if the templates break but never runs at runtime

## Mocks

- `GameServiceMocks.h`: `UMockGameServiceBase` (tracks `bWasStarted`, `bIsTickable`, `bWasShutDown`, `TickCounter`, plus `WasStartedBefore/After`, `WasShutdownBefore/After` ordering helpers) and concrete mocks `UMockGameService_Void`, `UMockGameService_Void2`, `UMockGameService_Interfaced` (+ `IMockGameServiceInterface`), `UMockGameService_VoidObserver`, `UMockGameService_VoidObserverAssistant`, `UMockGameService_VoidObserverFan`, each declaring dependencies via `ConfigureGameServiceUser()` to test dependency-graph ordering
- `GameServiceUserMocks.h`: `UGameServiceUserMock` exposes the normally-protected `FGameServiceUser` API (`UseGameService`, `UseGameServiceAsPtr`, `UseGameServiceAsInterface`, `FindOptionalGameService`, `WaitForDependencies`, `StopWaitingForDependencies`, `InvalidateCachedDependencies`) for tests, plus `SimulateTick()` to manually pump pending dependency-wait callbacks

# Project Integration

- New services should get a matching spec here that exercises start order, shutdown order and dependency resolution using the existing mocks, see `GameServiceUser.spec.cpp` as a template
- Reuse `FScopedAutomationTestWorld` (from `WeekendUtils`) rather than writing a bespoke test world setup
