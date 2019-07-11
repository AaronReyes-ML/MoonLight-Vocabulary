// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TrampolineGameGameMode.h"
#include "TrampolineGameCharacter.h"

ATrampolineGameGameMode::ATrampolineGameGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ATrampolineGameCharacter::StaticClass();	
}
