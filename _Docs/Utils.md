# WeekendUtils

## Overview

- Base/foundation module of the plugin, framework-agnostic utilities shared by the other modules and by consumer projects
- No single design philosophy, this module is a grab-bag of building blocks rather than one framework
- `WeekendGameService` and `WeekendSaveGame` were originally part of this module and got split out later
    - `Config/DefaultWeekendUtils.ini` still carries `CoreRedirects` for their old `/Script/WeekendUtils.*` class paths

## Architecture

## Automation Test Helpers

- `WeekendUtils::FScopedAutomationTestWorld` creates a full `UWorld` + `UGameInstance` for automation specs and tears it down on destruction
    - `InitializeGame()` / `InitializeGame(FConfig)` spawns game mode, viewport, local player and player controller so specs can exercise actor and gameplay-framework code
    - `FConfig` allows a custom `FURL` and `PlayerStateClass`
    - This is the reusable per-test world helper used throughout `WeekendUtilsTests`
- `WE_BEGIN_DEFINE_SPEC(Name)` / `WE_END_DEFINE_SPEC(Name)` wrap UE's `BEGIN_DEFINE_SPEC` / `END_DEFINE_SPEC` and require a `SPEC_TEST_CATEGORY` macro defined beforehand
- `WE_SPEC_CASES_SIGNATURE_N` / `WE_SPEC_CASE_N` / `WE_SPEC_CASES_CODE_BEGIN_N` / `WE_SPEC_CASES_CODE_END` (N = 1 to 9) provide table-driven, parametrized spec cases
- `WeekendUtils::SpecStringUtils::ToString(...)` stringifies floats and vectors without `.` characters, so spec names do not get split into fake sub-categories in the session frontend
- `Mocks/SubsystemMocks.h` provides `UEngineSubsystemMock`, `UWorldSubsystemMock`, `UGameInstanceSubsystemMock`, `ULocalPlayerSubsystemMock`, only instantiated during automation testing

## Gameplay Abilities

- `UEnhancedAbilitySystemComponent` binds gameplay abilities directly to Enhanced Input actions
    - Configured via `DefaultAbilities` (`FInputActionBindableAbility`), `DefaultAttributes` (`FAttributeSetConfig`), `DefaultEffects`
    - Virtual hooks: `GiveDefaultAbilities()`, `InitDefaultAttributeSets()`, `ApplyDefaultEffects()`, `HandleGameplayTagsChanged()`
- `UEnhancedGameplayAbility` adds `BindToInputComponent` / `UnbindFromInputComponent` plus `HandleInputPressed` / `HandleInputReleased` BlueprintNativeEvents
- `FInputActionBindableAbility` links a `UInputAction` to a `TSubclassOf<UGameplayAbility>` plus level, and can build/give the ability spec
- `FAttributeSetConfig` pairs a `TSubclassOf<UAttributeSet>` with an init `UDataTable`, and defines the `ATTRIBUTE_ACCESSORS` convenience macro

## Gameplay Debugger

- `GENERATE_DEBUGGER_EXTENSION(Name)` / `GENERATE_DEBUGGER_CATEGORY(Name)` macros plus `FGameplayDebugger::RegisterExtension<T>()` / `RegisterCategory<T>()` template helpers (all gated behind `WITH_GAMEPLAY_DEBUGGER`)
- Extensions: `FGameplayDebuggerExtension_SelfDebugActor` (select yourself as debug actor), `FGameplayDebuggerExtension_ToggleUiVisibility` (toggle the top-level UMG widget)
- Categories: `FGameplayDebuggerCategory_GameFeatures` (Game Feature plugin state), `FGameplayDebuggerCategory_InputActionAbilities` (abilities bound via `UEnhancedAbilitySystemComponent`)
- All four are registered in `FWeekendUtilsModule::StartupModule()` and unregistered in `ShutdownModule()`
- Note: the `GameServices` gameplay debugger category lives in `WeekendGameService`, not here

## Utils

- `UWeekendUtilsFunctionLibrary` (blueprint function library): `FindClosestComponentOnActorToWorldLocation`, `FindClosestActorToWorldLocation`, `FindClosestObjectToWorldLocation`
- `TObjectListSynchronizer<ObjectType, InitListType>` syncs an array of objects (e.g. a view model list) against an arbitrary data array, with fluent `ForEachMissingElement` / `ForEachUpdatedElement` / `ForEachRemovedElement` callbacks
- `EnumUtils.h`: `WeekendUtils::EnumToString<T>()` / `EnumFromString<T>()`, plus `DEFINE_ENUM_STRING_CONVERTERS` and `DEFINE_ENUM_BOOL_CONVERTERS` macros for generating `LexToString` / `LexFromString` overloads on any `enum class`
- Common shared enums built with those macros: `ECommonAvailability`, `ECommonEnabledState`, `ECommonHandledReply`, `ECommonValidity`, `ECommonYesNo`

# Debugging

- Gameplay Debugger categories `GameFeatures` and `InputActionAbilities` (toggle via the in-game Gameplay Debugger)
- Gameplay Debugger extensions `SelfDebugActor` and `ToggleUiVisibility`

# Project Integration

- This module has no dependency on `WeekendGameService`, `WeekendSaveGame` or `WeekendCheatMenu`, all other modules depend on it
- Use `FScopedAutomationTestWorld` and the `WE_*` spec macros as the standard way to write automation specs for gameplay code in a consuming project, see `WeekendUtilsTests` for examples
