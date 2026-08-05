# WeekendSaveGame

## Motivation

- Non-disruptive, non-destructive integration into projects
- Easy to use, easy to customize
- Data-driven, modular data containers
- Less dependencies, less boilerplate
- Less waiting and asynchronous callbacks
- UE Editor utilities assist developing with save games

## Philosophy

- One service to rule them all: `USaveGameService`
- Polymorphism: Project specific save/load behavior and serialization implementations
- Flexibility: Use any `USaveGame` implementation you want, but consider using `UModularSaveGame`
- Objects with restorable state don't need to know when saving and loading happens, they must only keep their state up-to-date

## Planned Improvements

- For big save games: Preload only header data to reduce memory usage
- Improved save/load request handling: See `USaveGameService::ProcessPendingRequests`

## Terminology

### Saving

- The process of serializing + writing the current `USaveGame` object to a file

### Loading

- The process of reading and deserializing a save game file to a `USaveGame` object
- In most cases, this also means restoring the save game

### Restoring

- The process of applying the state stored in a `USaveGame` object to the game, thus recreating the saved game state
- This does not include traveling to the stored game map

### Traveling

- Usually, the save game stores which map it was saved in. Traveling into a restored save game means loading into this stored map
- Traveling can only be done for restored save games, which means that once the new level has loaded, all relevant objects in that world will restore their state from the save game

### Preloading

- The process of reading and deserializing a save game file to a `USaveGame` object, without restoring its state
- This is usually done at application start, to populate the list of save games the player can load from, and to determine the most recent save game for continuing

# Architecture

## USaveGameService

- Central API for saving, loading and restoring save games
- Maintains lifetime of `USaveGame` objects
- Configurable via `USaveGameServiceSettings` in project settings
- Started by the first initialized world - before all actors receive `BeginPlay()`
- Shut down together with the game instance - after all actors receive `EndPlay()`
- Provides events for saving and restoring the game's state
- Provides API to lock/unlock saving and loading from game code

## USaveLoadBehavior

- Polymorphic sub-object, created and maintained by `USaveGameService`
- Knows about the used save game implementation and how data is written and read from it
- Behavior classes used in different game environments are defined via `USaveGameServiceSettings` in project settings:
    - SaveLoadBehavior
    - PlayInStandaloneSaveLoadBehavior
    - PlayInEditorSaveLoadBehavior
    - AutomationTestSaveLoadBehavior
- It is recommended to create your own behavior classes early on, deriving from the default implementations
- The default implementations define `UModularSaveGame` as save game class implementation
- Defines which `USaveGameSerializer` class is used by the `USaveGameService`

### UDefaultSaveLoadBehavior

- Defines 8 save game slots:
```cpp
SaveSlotNames =
{
	"GameSlot1", "GameSlot2", "GameSlot3", "GameSlot4",
	"GameSlot5", "GameSlot6", "GameSlot7", "GameSlot8",
};
```
- Asynchronously preloads all save games at game start, then restores the most recent one as "CurrentSaveGame"
    - However: does not automatically travel to the restored game map
    - You can use `UCurrentSaveGameViewModel` in your main menu to provide a "Continue" or "New Game" button

### UDefaultPlayInEditorSaveLoadBehavior

- Uses editor-relevant config values defined in the `USaveGameServiceSettings` in project settings
- By default, saves to and loads from "PlayInEditor" slot
    - This slot can also be overwritten locally by the `EW_PlayInEditorSaveGame` editor utility widget
- Loads and restores synchronously from the save game slot
    - This means that even in PIE, the save game is always there and level objects can start with a properly restored state without having to wait

## Save Game Presets

- Save game presets are data assets that contain an instanced `USaveGame` object plus (optionally) its header data
- They can be created manually or via the Save Game Editor by converting the current save game to a preset
- Presets should be located in the "DefaultSaveGamePresetFolder" that can be configured in the `USaveGameServiceSettings` in project settings
- Presets are development only and not available in shipping builds
- Presets can be loaded like save game files, but are part of the project files
    - In the editor, presets can be loaded by the `EW_PlayInEditorSaveGame` utility widget
- It is recommended to have only few presets, since they must be maintained as the game grows together with the saved data
- Example presets:
    - "100% Unlocked" restores a game state where all features are unlocked
    - "Final Mission" restores a game state where all missions except the final one have been played, so developers can easily test it without grinding through multiple cheats first

## Save Game Header

- Headers are supposed to provide meta information about the save game (like a compatibility version, when it was last saved or loaded, ...)
- By default, custom save game headers are only supported by the `UModularSaveGame` implementation
- `FSimpleSaveGameHeaderData` is a default implementation that contains various meta information and which level the game was saved in
    - This struct is used by the `UModularSaveGame`
- It is recommended to think about which data to store here as early as possible, to avoid incompatibilities later on

## Modular Save Game

- `USaveGame` implementation that uses polymorphic subobjects called "SaveGameModules"
- Supports custom header data (as described above)
- Game systems and level objects can derive their runtime state objects from `USaveGameModule` and register them with the modular save game
    - The state-keeper and the save game both keep a reference to the module
    - The state-keeper reads its initial state from its state object after registering it
    - The state-keeper updates the state at runtime so it is correctly saved each time the save game is saved
- Modules provide delegates for when they are saved and restored, in case state-keepers need to implement custom logic for saving or restoring
- Properties of modules should be marked with the "SaveGame" UPROPERTY specifier
- It is recommended to make relevant properties EditInstanceOnly, so they can be edited in the Save Game Editor (see section: Debugging)
- Example module:
```cpp
UCLASS(DisplayName = "Scenario Service State")
class WEEKENDSCENARIO_API UScenarioServiceState : public USaveGameModule
{
	GENERATED_BODY()

public:
	UScenarioServiceState()
	{
		DefaultModuleName = "ScenarioService";
		ModuleVersion = 0;
	}

	UPROPERTY(SaveGame, VisibleAnywhere, meta = (ForceInlineRow, ShowOnlyInnerProperties))
	TMap<FScenarioTaskId, FActiveScenarioTaskData> ScenarioTaskStates = {};
};

void UScenarioService::StartService()
{
	// Service stores the state like this in the header:
	// UPROPERTY()
	// TObjectPtr<UScenarioServiceState> CurrentState = nullptr;
	
	// Create the module, cache and then initialize the service with its data:
	CurrentState = &UModularSaveGame::SummonModule<UScenarioServiceState>(*this);
}
```

# Debugging

## EW_PlayInEditorSaveGame

![SCRN_20240318_135616_001](https://github.com/barzb/UnrealWeekendUtils/assets/14543036/507d5575-fda4-403f-8358-30a94ae2ecff)
- Editor utility widget that can be found at `/Plugins/WeekendUtils/SaveLoad/EW_PlayInEditorSaveGame`
- Can also be run via menu bar: Tools → Editor Utility Widgets → PIE SaveGame
- When enabling the `Restore Custom SaveGame` checkbox, you can pick a local save game file or preset to restore when starting PIE
    - This setting is saved locally only for your editor instance

## USaveGameEditor

![image](https://github.com/barzb/UnrealWeekendUtils/assets/14543036/e1051d75-3778-49ae-8ac6-fb4ce1d19920)
- Can be opened via cheat: `Cheat.SaveGame.OpenEditor` or via `EW_PlayInEditorSaveGame` utility widget
- Pressing `Edit Current Save Game` will display the currently loaded `USaveGame` object instance in the editor
- You can `Convert to Preset` which will open a save-asset dialog pointing to the configured `DefaultSaveGamePresetFolder` (project settings)

# Project Integration

## View Models

- This plugin comes with some handy [view models](https://github.com/barzb/WeekendUtils/tree/main/Source/WeekendSaveGame/Public/SaveGame/ViewModels) classes for save game slots, list and a status marker

## USaveGameActorComponent & ULevelObjectRestorer

- `ULevelObjectRestorer` is a `USaveGameModule` that can save and restore objects nested somewhere within a game world (actors, components, game modes, player states, ...)
    - Registered objects need to provide a unique ID that can identify them against other objects, so states are restored properly
    - Registered objects will automatically save and restore all properties with the "SaveGame" UPROPERTY specifier
- `USaveGameActorComponent` is a component designed to take over level object registration in a convenient fashion
    - It can be slapped on to any level actor (NOT runtime spawned actor) and allows various customization options via its settings
