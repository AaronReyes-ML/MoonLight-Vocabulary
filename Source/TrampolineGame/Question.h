// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>

/**
 * 
 */
class TRAMPOLINEGAME_API Question
{
public:
	FString kanji = "";
	FString hiragana = "";
	FString english = "";
	FString wordType = "";
	int wordTypeCode = 0;
	int timesAnsweredCorrectly = 0;
	bool justAnsweredCorrectly = false;

	TArray<FString> definitionList;
	int currentDefinitionListIndex = 0;

	void SetTimesAnsweredCorrectly(int val);
	void SetJustAnsweredCorrectly(bool val);
	Question();
	~Question();
};
