﻿///////////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) by Benjamin Barz and contributors. See file: CREDITS.md
///
/// This file is part of the WeekendUtils UE5 Plugin.
///
/// Distributed under the MIT License. See file: LICENSE.md
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "SaveGame/SaveGameModule.h"

#include "LevelObjectRestorer.generated.h"

///////////////////////////////////////////////////////////////////////////////////////

/** Implementation detail of @USaveGameModule_LevelObjects. */
USTRUCT()
struct WEEKENDUTILS_API FLevelObjectSaveGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Weekend Utils|Save Game")
	int32 ByteDataSize = 0;

	UPROPERTY(SaveGame)
	TArray<uint8> ByteData = {};
};

///////////////////////////////////////////////////////////////////////////////////////

/**
 * Saves and restores objects (Actors, Components or UObjects) that are part of a level.
 * By default, objects are matched by their PathName. Changing the name in the world outliner can break data of older save games.
 * Runtime spawned objects (like GameMode, GameState, ...) MUST provide a CustomUniqueObjectId to make them identifiable.
 * @requires UModularSaveGame
 */
UCLASS(DisplayName = "Level Objects")
class WEEKENDUTILS_API ULevelObjectRestorer : public USaveGameModule
{
	GENERATED_BODY()

public:
	ULevelObjectRestorer()
	{
		DefaultModuleName = "LevelObjectRestorer";
		ModuleVersion = 0;
	}

	/**
	 * Registers an object nested somewhere within a world to save and restore it's data from the @UModularSaveGame.
	 * By default, objects immediately restore their data upon registration (@bImmediatelyRestoreIfPossible).
	 * Only saves and restores properties with the "SaveGame" specifier. Does not automatically save and restore sub-objects.
	 * Objects that are not uniquely identifiable via the objects PathName (like runtime spawned objects) MUST provide a CustomUniqueObjectId.
	 */
	void RegisterLevelObject(UObject& Object, TOptional<FString> CustomUniqueObjectId = {}, bool bImmediatelyRestoreIfPossible = true);
	void RegisterLevelObjectWithTransform(AActor& Actor, TOptional<FString> CustomUniqueObjectId = {}, bool bImmediatelyRestoreIfPossible = true);
	void RegisterLevelObjectWithTransform(USceneComponent& SceneComponent, TOptional<FString> CustomUniqueObjectId = {}, bool bImmediatelyRestoreIfPossible = true);

	/**
	 * Deregisters an previously registered object. This should be called when the object dies.
	 * By default, the saved object state will be updated and kept in the @UModularSaveGame and can be restored once the object registers again (@bKeepObjectState).
	 * Objects that are not uniquely identifiable via the objects PathName (like runtime spawned objects) MUST provide a CustomUniqueObjectId.
	 */
	void UnregisterLevelObject(UObject& Object, TOptional<FString> CustomUniqueObjectId = {}, bool bKeepObjectState = true);
	void UnregisterLevelObjectWithTransform(AActor& Actor, TOptional<FString> CustomUniqueObjectId = {}, bool bKeepObjectState = true);
	void UnregisterLevelObjectWithTransform(USceneComponent& SceneComponent, TOptional<FString> CustomUniqueObjectId = {}, bool bKeepObjectState = true);

	// - UObject
	virtual void Serialize(FArchive& Ar) override;
	// --

protected:
	UPROPERTY(Transient, VisibleAnywhere, meta = (DisplayThumbnail = "false"), Category = "Weekend Utils|Save Game")
	TSet<TWeakObjectPtr<UObject>> SimpleRegisteredObjects = {};
	UPROPERTY(Transient, VisibleAnywhere, meta = (DisplayThumbnail = "false"), Category = "Weekend Utils|Save Game")
	TSet<TWeakObjectPtr<UObject>> RegisteredObjectsWithTransform = {};
	UPROPERTY(Transient, VisibleAnywhere, meta = (DisplayThumbnail = "false"), Category = "Weekend Utils|Save Game")
	TMap<TWeakObjectPtr<UObject>, FString> UniqueIdsOfRegisteredObjects = {};

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Weekend Utils|Save Game")
	TMap<FString, FLevelObjectSaveGameState> ObjectStates;

	virtual void SaveObjectToState(UObject& Object, bool bSaveTransform, FLevelObjectSaveGameState& InOutState) const;
	virtual void RestoreObjectFromState(const FLevelObjectSaveGameState& State, bool bRestoreTransform, UObject& InOutObject) const;

	virtual FTransform GetObjectTransform(UObject& Object) const;
	virtual void SetObjectTransform(UObject& Object, const FTransform& Transform) const;
};
