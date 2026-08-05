## WeekendUtils

Unreal Engine 5 plugin collecting reusable gameplay-framework modules: a game service framework (dependency-driven, mockable singleton services), a save game framework (modular, data-driven save games), a cheat menu, and shared utility code.

**Table of Contents:**
- [Modules](#modules)
- [Documentation](#documentation)
- [Supported Engine Version](#supported-engine-version)
- [Installation](#installation)
- [Repos that use WeekendUtils](#repos-that-use-weekendutils)
- [Update notes](#update-notes-05082026)

### Modules
- `WeekendUtils` - shared utility code
- `WeekendGameService` - game service framework
- `WeekendSaveGame` - save game framework
- `WeekendCheatMenu` - in-game cheat menu
- `WeekendUtilsTests` - automation test specs / usage examples

### Documentation
- [WeekendUtils](_Docs/Utils.md)
- [Game Service Framework](_Docs/GameServiceFramework.md)
- [Save Game Framework](_Docs/SaveGameFramework.md)
- [Cheat Menu](_Docs/CheatMenu.md)
- [WeekendUtilsTests](_Docs/UtilsTests.md)

### Supported Engine Version

The plugin was developed for Unreal Engine 5.8+, though it should work for all 5.X versions.

### Installation

Plugin lives at repo root (`WeekendUtils.uplugin` in root, not nested `Plugins/` folder). Two ways to add it to your project or engine:

**1. Clone directly into Plugins folder**

Project-only:
```
cd YourProject/Plugins
git clone https://github.com/barzb/WeekendUtils.git
```

Engine-wide (all projects on that engine install):
```
cd UE_5.x/Engine/Plugins
git clone https://github.com/barzb/WeekendUtils.git
```

Regenerate project files / restart editor after.

**2. Add as git submodule**

From your project root (submodule path must live under `Plugins/`):
```
git submodule add https://github.com/barzb/WeekendUtils.git Plugins/WeekendUtils
git submodule update --init --recursive
```

Cloning your project fresh afterwards, pull submodule content too:
```
git clone --recurse-submodules <your-project-repo-url>
```
or, if already cloned without it:
```
git submodule update --init --recursive
```

To update plugin to latest upstream commit later:
```
cd Plugins/WeekendUtils
git pull origin main
cd ../..
git add Plugins/WeekendUtils
git commit -m "Update WeekendUtils submodule"
```

### Repos that use WeekendUtils

- [WeekendScenario](https://github.com/barzb/WeekendScenario) - Graph-based scenario/quest/mission framework
- [SFConditional](https://github.com/Strayfarer/SFConditional) - Polymorphic conditional trees for UE5 with binary and fuzzy evaluation.

### **Update notes: 05/08/2026**
**Breaking Changes to the GameService framework!**
- ```UGameServiceManager``` is not a ```UEngineSubsystem``` anymore!
  - Instead, it is now tied to a ```UGameInstance```, so local service environments in multiplayer PIE sessions are supported. This is in preparation for a bigger multiplayer support of the framework (TBD).
  - ```FGameServiceUser``` derived classes must now implement ```ConfigureGameServiceUser()``` to configure their dependencies, instead of doing so in their constructor.
    ```
    virtual FGameServiceUserConfig UExample::ConfigureGameServiceUser() const override
    {
      return FGameServiceUserConfig(this)
        .AddServiceDependency<USaveGameService>();
    }
    ```
  - ```UseGameService()``` and similar methods do not need to pass ```this``` anymore.
    ```
    void UExample::DoSomething()
    {
      UseGameService<USaveGameService>().RequestAutosave("example");
    }
    ```
- Used dependencies in ```FGameServiceUser``` derived classes are now cached after first look-up.
- Renamed some static getters in ```UGameServiceManager``` and ```UModularSaveGame```.
- Fixed and improved some broken unit tests and adjusted ```FScopedAutomationTestWorld``` to accommodate for the changes in the GameService framework.
- ```UGameServiceLocator``` now needs a ```WorldContext``` passed for all static locator methods.
