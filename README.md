## WeekendUtils

Unreal Engine 5 plugin collecting reusable gameplay-framework modules: a game service framework (dependency-driven, mockable singleton services), a save game framework (modular, data-driven save games), a cheat menu, and shared utility code.

**Modules:**
- `WeekendUtils` - shared utility code
- `WeekendGameService` - game service framework
- `WeekendSaveGame` - save game framework
- `WeekendCheatMenu` - in-game cheat menu
- `WeekendUtilsTests` - automation test specs / usage examples

**Documentation:**
- [WeekendUtils](_Docs/Utils.md)
- [Game Service Framework](_Docs/GameServiceFramework.md)
- [Save Game Framework](_Docs/SaveGameFramework.md)
- [Cheat Menu](_Docs/CheatMenu.md)
- [WeekendUtilsTests](_Docs/UtilsTests.md)

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
