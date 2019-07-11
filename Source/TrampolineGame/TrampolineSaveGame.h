// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LevelPrompt.h"
#include "TrampolineSaveGame.generated.h"

USTRUCT()
struct FLevelCriticalInformationStruct
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	bool completed;
	UPROPERTY(EditAnywhere)
	int score;
	UPROPERTY(EditAnywhere)
	int questionsCorrect;
	UPROPERTY(EditAnywhere)
	int starsEarned;

	FLevelCriticalInformationStruct()
	{
		completed = false;
		score = 0;
		questionsCorrect = 0;
		starsEarned = 0;
	}
	FLevelCriticalInformationStruct(bool c, int s, int q, int str)
	{
		completed = c;
		score = s;
		questionsCorrect = q;
		starsEarned = str;
	}
};
/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API UTrampolineSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere)
	int playerTotalStars = 0;
	UPROPERTY(VisibleAnywhere)
	int playerTotalAutoFeverHeld = 0;
	UPROPERTY(VisibleAnywhere)
	int playerTotalQuestionReduceHeld = 0;
	UPROPERTY(VisibleAnywhere)
	int playerTotalSupportHeld = 0;

	UPROPERTY(VisibleAnywhere)
	TArray<FLevelCriticalInformationStruct> levelPromptArray;

	UPROPERTY(VisibleAnywhere)
	TArray<bool> itemPickupArray;

	UPROPERTY(VisibleAnywhere)
	TArray<bool> starGateArray;

	UPROPERTY(VisibleAnywhere)
	FString SaveSlotName;
	UPROPERTY(VisibleAnywhere)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere)
	float savedCharacterDistanceOnSpline = 0;
	UPROPERTY(VisibleAnywhere)
	int savedJunctionID = 0;
	UPROPERTY(VisibleAnywhere)
	int savedSplineID = 0;
	UPROPERTY(VisibleAnywhere)
	FString savedLevelPromptTag = "";
	UPROPERTY(VisibleAnywhere)
	int savedLevelPromptID = 0;

	UPROPERTY(VisibleAnywhere)
	int levelID = 0;
	UPROPERTY(VisibleAnywhere)
	int previousLevelID = -1;

	UPROPERTY(EditAnywhere)
	int levelKeysHeld = 0;

	UTrampolineSaveGame();
};
