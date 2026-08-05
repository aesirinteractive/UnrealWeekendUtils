# WeekendCheatMenu

## Motivation

- Define a cheat once, get both a console command and a menu entry for it
- Cheats should be easy to discover, search and re-run during development
- Typed arguments with descriptions, instead of raw string parsing

## Philosophy

- `ICheatMenuAction` is the base for anything listed in the cheat menu
- `ICheatCommand` extends it with an auto-registered console command (`ECVF_Cheat`)
- Cheats are grouped into `FCheatCommandCollection`s, which can optionally show up as a tab/section in the menu
- Variants let you pre-fill arguments for a cheat (e.g. `+100 Score` as a shortcut for `Cheat.Score.Increase 100`), including variants generated at runtime

## Architecture

## ICheatMenuAction

- Abstract base for menu-listable actions, not necessarily a console command
- `FDescriber` builder configures display name, description and typed arguments (`DisplayAs`, `DescribeCheat`, `DescribeArgument<T>`, `DescribeArgumentWithOptions`)
- `FArgumentInfo` describes a single argument (name, description, `EArgumentStyle`: Number, FloatNumber, Text, TrueFalse, DropdownText)
- Protected helpers available inside `Execute()`: `LogInfo/Warning/Error/Verbose/VeryVerbose`, `GetLocalPlayerController/State/Pawn<T>()`, `GetNextArgumentOr<T>()`, `GetNextArgumentOrError<T>()`, `GetOrCreateSharedContextObjectForWorld()`

## ICheatCommand

- Extends `ICheatMenuAction` with an `FAutoConsoleCommandWithWorldAndArgs`, so the same action is invocable from the UE console

## Cheat Command Collections

- `Cheats::FCheatCommandCollection` groups related cheats and self-registers in `Cheats::GetAllCollections()`
- Optional `FCheatMenuCategorySettings` (fluent `.Tab(Name).Section(Name)`) controls if/where the collection shows up in the menu

## Cheat Variants

- `FCheatCommandVariant` proxies an existing cheat command with pre-filled argument overrides
- `BEGIN_DEFINE_CHEAT_VARIANTS` / `DEFINE_CHEAT_VARIANT` / `END_DEFINE_CHEAT_VARIANTS` support conditional variant generation at runtime (e.g. PIE-only variants)

## Cheat Menu UI

- `SCheatMenu` (Slate) renders tabs/sections, typed argument input widgets, search, "Favorites" and "Recently Used"
    - Favorites, recently used and last-used arguments are persisted per project into `GameUserSettings.ini`, section `WeekendUtils.CheatMenu`
- `UCheatMenu` (UMG `UWidget`) wraps `SCheatMenu` for embedding in a project's own UI
    - `bShouldCloseAfterCheatExecution` and `OnCloseRequested` let a parent widget close a popup after a cheat runs

# Debugging

- `Cheat.OpenCheatMenu` opens the Slate cheat menu in a standalone window
- `Cheat.WriteCheatListToFile` exports all registered cheats and their argument descriptions to a CSV file (defaults to the project's Saved directory), useful as a generated cheat sheet

# Project Integration

## Defining a Cheat

```cpp
DEFINE_CHEAT_COLLECTION(ExampleCheats, AsCheatMenuTab("Tab1").Section("Misc"))
{
    DEFINE_CHEAT_COMMAND(IncreaseScoreCheat, "Cheat.Score.Increase")
    .DisplayAs("Increase Score")
    .DescribeCheat("Increases the player score by a certain amount.")
    .DescribeArgument<int32>("Amount", "The additional score. Default: 100")
    DEFINE_CHEAT_EXECUTE(IncreaseScoreCheat)
    {
        const int32 Amount = GetNextArgumentOr<int32>(100);
        UScoreSystem::AddScore(Amount);
        LogInfo("Score was increased by " + FString::FromInt(Amount));
    }
}
```

- This expands into a generated `ICheatCommand` derived class, instantiated once per translation unit and registered with its collection in the constructor
- Class hierarchy: `ICheatMenuAction` (menu only) then `ICheatCommand` (adds console command) then `FCheatCommandVariant` (proxy with preset args)

## Where Other Modules Hook In

- `WeekendSaveGame` defines `Cheat.SaveGame.Autosave`, `Cheat.SaveGame.LoadAutosave` and (editor only) `Cheat.SaveGame.OpenEditor`, which opens `USaveGameEditor`
- `WeekendCheatMenu` itself has no dependency on `WeekendGameService`, `WeekendSaveGame` or `WeekendUtils`, it is a leaf module consumed by the others
