// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question.h"
#include <vector>
#include <string>

/**
 *
 */
class TRAMPOLINEGAME_API QuestionManager
{
	Question currentQuestion;

protected:
	int currentQuestionCorrectAnswerIndex = -1;
	int currentQuestionIndex = -1;

	int totalQuestions = -1;
	int numberOfQuestionsAnsweredCorreclty = 0;

	bool CompareStringContents(FString stringA, FString stringB);

public:

	QuestionManager();
	~QuestionManager();

	bool lastQuestionAnsweredCorrectly = false;

	int GetQuestionMode();

	int fileCode = 0;
	int startIndex = 0;
	int totalQuestionsToRead = -1;
	int minQuestionsToWin = -1;
	int wordTypeMode = -1;
	int questionMode = 0;

	std::vector<Question> questionVector;
	std::vector<Question> totalResourceVector;
	std::vector<Question> allQuestionVector;
	std::vector<Question> nounsVector;
	std::vector<Question> adjsVector;
	std::vector<Question> verbsVector;
	std::vector<Question> answerVector;
	std::vector<Question> dummyAnswerVector;

	std::vector<int> usedDummyIndexes;

	bool isFreePlay = false;

	bool FileParser(std::string filename);
	bool FileParser2(class UDataTable* dataTable);
	int WordTypeParser(std::string wordTypeString);
	Question GetCurrentQuestion();
	void GetNewQuestion();
	void GetNewQuestion2();
	void GetDummyAnswers(int index);
	void GetDummyAnswers2();
	void SetRandomCorrectAnswerIndex();
	bool CheckUsedDummyIndexes(int index);
	bool CheckUsedDummyIndexes2(int index);

	FString GetNextDef(Question &q);
	FString GetPrevDef(Question &q);

	int GetCorrectIndex();
	int GetCurrentQuestionIndex();

	void IncrementQuestionsAnsweredCorrectly();
	int GetNumberOfQuestionsAnsweredCorrectly();
	int GetTotalNumberOfQuestions();

	int myrandom(int i);
};
