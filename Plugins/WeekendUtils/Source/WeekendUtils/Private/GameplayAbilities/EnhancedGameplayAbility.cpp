///////////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) by Benjamin Barz and contributors. See file: CREDITS.md
///
/// This file is part of the WeekendUtils UE5 Plugin.
///
/// Distributed under the MIT License. See file: LICENSE.md
///
///////////////////////////////////////////////////////////////////////////////////////

#include "GameplayAbilities/EnhancedGameplayAbility.h"

#include "Engine/InputDelegateBinding.h"

void UEnhancedGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	HandleInputPressed(*ActorInfo, ActivationInfo);
}

void UEnhancedGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	HandleInputReleased(*ActorInfo, ActivationInfo);
}

void UEnhancedGameplayAbility::BindToInputComponent(UInputComponent* InputComponent)
{
	if (!InputComponent)
	{
		InputComponent = GetOwningActorFromActorInfo()->FindComponentByClass<UInputComponent>();
	}

	if (ensure(IsInstantiated()))
	{
		UInputDelegateBinding::BindInputDelegates(GetClass(), InputComponent, this);
	}
}

void UEnhancedGameplayAbility::UnbindFromInputComponent(UInputComponent* InputComponent)
{
	if (!InputComponent)
	{
		InputComponent = GetOwningActorFromActorInfo()->FindComponentByClass<UInputComponent>();
		if (!InputComponent)
			return;
	}

	if (ensure(IsInstantiated()))
	{
		InputComponent->ClearBindingsForObject(this);
	}
}

void UEnhancedGameplayAbility::HandleInputPressed_Implementation(const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo)
{
}

void UEnhancedGameplayAbility::HandleInputReleased_Implementation(const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo)
{
}
