// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrampolineMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API UTrampolineMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void DoTransitionToLevel();
};
