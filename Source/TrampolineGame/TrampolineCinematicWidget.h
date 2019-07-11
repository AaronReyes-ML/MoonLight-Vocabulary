// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CinematicPawn.h"
#include "TrampolineCinematicWidget.generated.h"

/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API UTrampolineCinematicWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowNewText(int index);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EnableProgression();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DisableProgression();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Transition(bool end);


	
};
