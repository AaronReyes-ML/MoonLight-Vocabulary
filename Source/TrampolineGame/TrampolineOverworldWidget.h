// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelPrompt.h"
#include "StarGate.h"
#include "ItemPickup.h"
#include "TrampolineOverworldWidget.generated.h"

/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API UTrampolineOverworldWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowLevelPrompt(ALevelPrompt* levelPrompt);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideLevelPrompt();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DoTransitionToLevel();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateTotalStarCount(int starCount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdatePowerupCount(int autoFever, int questionReduce, int support);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowStarGatePrompt(AStarGate* starGate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideStarGatePrompt();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowItemPickupPrompt(AItemPickup* itemPickup);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideItemPickupPrompt();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ReturnToMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DoTransitionToOverworldLevel(int levelID);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateLevelText(int levelID);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DoAcknowledgeLevelImage();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateBossKeyCount(int bossKeys);
};