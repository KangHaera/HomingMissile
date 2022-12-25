// Copyright Epic Games, Inc. All Rights Reserved.

#include "HMGameMode.h"

#include "UObject/ConstructorHelpers.h"

#include "HomingMissile/HUD/HMHUD.h"
#include "HomingMissile/Player/HMCharacter.h"

AHMGameMode::AHMGameMode()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/Player.Player_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = AHMHUD::StaticClass();

}
