// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include <string>
#include "TrampolineGameState.generated.h"

/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API ATrampolineGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	std::string fileName = "";
	
};
