// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Question.h"
#include "TrampolineGameWidget.generated.h"

/**
 *
 */
UCLASS()
class TRAMPOLINEGAME_API UTrampolineGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateHealthUI(float health);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdatePercentageUI(float progess);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateComboUI(int combo, int extra);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateAltitudeOffsetUI(float alt, bool extending, int currentAlt, int maxAlt);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateJumpLevelUI(int jumpLevel);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateFeverUI(float fever, bool feverReady, bool inFever, int progressCounter);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateTranscendanceUI(float transcendance, bool inTranscendance, int progressCounter);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateScoreUI(int score);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdatePowerupUI(int autoFever, int questionReduce, int support);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateCurrentQuestion(const FString &question);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateCurrentAnswers(const FString &ans1, const FString &ans2, const FString &ans3, const FString &ans4, const FString &ans5);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateQuestionHistory(const FString &hist1_val1, const FString &hist1_val2, bool hist1_correct, const FString &hist2_val1, const FString &hist2_val2, bool hist2_correct, const FString &hist3_val1, const FString &hist3_val2, bool hist3_correct, const FString &hist4_val1, const FString &hist4_val2, bool hist4_correct, const FString &hist5_val1, const FString &hist5_val2, bool hist5_correct);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowGameOver(bool cleared, int qCorrect, int qNeeded, int score, int star1, int star2, int star3);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowGameWon(bool cleared, int qCorrect, int qNeeded, int score, int star1, int star2, int star3);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowGamePassed();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowGamePaused(bool show, bool cleared, int qCorrect, int qNeeded, int score, int star1, int star2, int star3);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateTotalQuestionsUI(int correct, int target, int total);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void HideLaunchUI();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowGameUI();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateLaunchUI(float meterPercent1, bool meterCommit1, float meterPercent2, bool meterCommit2);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowDetailedHistoryUI(bool show);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void HighlightAnswers(int index1, int index2, int index3);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ResetAnswerColors();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void DoAnswerPulse(bool correct);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateStarTargets(int target1, int target2, int target3);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void DisableAllGameplayButtons();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void EnableAllGameplayButtons();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateJumpCooldownBar(float percent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void DoSupportAnimation(int supportIndex);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowRewardUI(int countAwarded);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void DoAcknowledgeMode(int correctAnswerIndex);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ExitAcknowledgeMode(int correctAnswerIndex);
};
