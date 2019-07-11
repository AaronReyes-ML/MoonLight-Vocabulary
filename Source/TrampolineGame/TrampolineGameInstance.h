// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Question.h"
#include <vector>
#include <string>
#include "TrampolineGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API UTrampolineGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	
public:
	// Game state information
	bool cameFromOverworldLevel = false;
	bool cameFromLevel = false;
	bool wasLevelCompleted = false;
	int previousLevelScore = -1;
	int previousLevelQuestionsCorrect = -1;
	int previousLevelStarsEarned = -1;
	int previousLevelAutoFeverRemain = 0;
	int previousLevelQuestionReduceRemain = 0;
	int previousLevelSupportRemain = 0;
	int levelID = 0;
	bool levelWasBossLevel = false;

	float savedCharacterDistanceOnSpline = -1;
	FString savedJunctionTag = "";
	int savedSplineID = -1;
	int savedLevelPromptID = 0;
	int autoFeverHeld = 0;
	int questionReduceHeld = 0;
	int supportHeld = 0;

	FString instancedSaveSlotName = TEXT("SaveSlot1");

	// Level control
	// Is free play or campaign
	bool isFreePlay = false;
	// Details for current level
	// 5 for N5, 4 for N4, 3 for N3, 2 for N2, 1 for N1
	int fileCode = -1;
	// Start index into the question vector
	int startIndex = -1;
	// Total questions to read from start
	int totalQuestionsToRead = -1;
	// Questions correct to win
	int minQuestionsToWin = -1;
	// Questions incorrect to lose
	int minQuestionsToLose = -1;
	// -1 All, 0 Noun, 1 Adj, 2 Verb
	int wordTypeMode = -1;
	// 0 for Japanese Kanji/Hiragana with English answers
	// 1 For Japanase Kanji with Hiragana answers
	// 2 For Japanese Kanji/English with Hiragana answers
	// 3 For English with Kanji answers
	int questionMode = 0;
	int scoreThresholdForClear = -1;
	int scoreThresholdFor2Star = -1;
	int scoreThresholdFor3Star = -1;

	bool musicActive = true;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsFreePlay() { return isFreePlay; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetLevelCode() { return isFreePlay; };

	UFUNCTION(BlueprintCallable)
	void SetMusicActive(bool a);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetMusicActive() { return musicActive; };
};
