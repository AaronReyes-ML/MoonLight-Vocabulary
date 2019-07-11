// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestionManager.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <ctime>
#include "Engine/DataTable.h"
#include "ConstructorHelpers.h"
#include "CSV_Question.h"
#include "Engine.h"

QuestionManager::QuestionManager()
{

}

QuestionManager::~QuestionManager()
{
}

bool QuestionManager::FileParser(std::string filename)
{
	return false;
}

bool QuestionManager::FileParser2(UDataTable* dataTable)
{
	TArray<FQData> tableInternalData;
	TArray<FName> rows = dataTable->GetRowNames();
	FString conString;

	Question currentQuestion = Question();
	for (int i = 0; i < rows.Num(); i++)
	{
		FQData* temp = dataTable->FindRow<FQData>(rows[i], conString, false);
		currentQuestion.kanji = temp->kanji;
		currentQuestion.hiragana = temp->hiragana;
		currentQuestion.english = temp->english;
		currentQuestion.wordType = temp->wordtype;
		currentQuestion.wordTypeCode = WordTypeParser(TCHAR_TO_UTF8(*temp->wordtype));

		FString defSubstring = "";
		bool encounteredUnPairedParen = false;
		for (int i = 0; i < currentQuestion.english.Len(); i++)
		{
			if (i == currentQuestion.english.Len() - 1)
			{
				defSubstring.AppendChar(currentQuestion.english[i]);
				currentQuestion.definitionList.Add(defSubstring.TrimStart());
			}
			else if (currentQuestion.english[i] == *UTF8_TO_TCHAR("("))
			{
				defSubstring.AppendChar(currentQuestion.english[i]);
				encounteredUnPairedParen = true;
			}
			else if (currentQuestion.english[i] == *UTF8_TO_TCHAR(")") && encounteredUnPairedParen)
			{
				defSubstring.AppendChar(currentQuestion.english[i]);
				encounteredUnPairedParen = false;
			}
			else if (currentQuestion.english[i] == *UTF8_TO_TCHAR(";") && i != 0 && !encounteredUnPairedParen)
			{
				currentQuestion.definitionList.Add(defSubstring.TrimStart());
				defSubstring = "";
			}
			else if (currentQuestion.english[i] != *UTF8_TO_TCHAR(";"))
			{
				defSubstring.AppendChar(currentQuestion.english[i]);
			}
		}

		if (currentQuestion.definitionList.Num() == 0)
		{
			currentQuestion.definitionList.Add("No item found");
		}

		allQuestionVector.push_back(currentQuestion);
		switch (currentQuestion.wordTypeCode)
		{
		case 0:
			nounsVector.push_back(currentQuestion);
			break;
		case 1:
			adjsVector.push_back(currentQuestion);
			break;
		case 2:
			verbsVector.push_back(currentQuestion);
			break;
		case 3:
			break;
		default:
			break;
		}
		currentQuestion = Question();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 99, FColor::Cyan, FString("Nouns ") + FString::FromInt(nounsVector.size()));
	//GEngine->AddOnScreenDebugMessage(-1, 99, FColor::Cyan, FString("Verbs ") + FString::FromInt(verbsVector.size()));
	//GEngine->AddOnScreenDebugMessage(-1, 99, FColor::Cyan, FString("Adjectives ") + FString::FromInt(adjsVector.size()));
	//GEngine->AddOnScreenDebugMessage(-1, 99, FColor::Cyan, FString("All ") + FString::FromInt(allQuestionVector.size()));

	switch (wordTypeMode)
	{
	case -1:
		questionVector = allQuestionVector;
		totalResourceVector = allQuestionVector;
		break;
	case 0:
		questionVector = nounsVector;
		totalResourceVector = nounsVector;
		break;
	case 1:
		questionVector = adjsVector;
		totalResourceVector = adjsVector;
		break;
	case 2:
		questionVector = verbsVector;
		totalResourceVector = verbsVector;
		break;
	default:
		break;
	}

	if (minQuestionsToWin == -1)
	{
		totalQuestions = questionVector.size();
		minQuestionsToWin = totalQuestions;
	}
	else
	{
		totalQuestions = minQuestionsToWin;
	}

	if (totalQuestionsToRead == -1)
	{
		totalQuestionsToRead = totalQuestions;
	}

	return true;
}

int QuestionManager::myrandom(int i)
{
	return 0;
}

FString QuestionManager::GetNextDef(Question &q)
{
	if (q.currentDefinitionListIndex < q.definitionList.Num() - 1)
	{
		q.currentDefinitionListIndex += 1;
		return q.definitionList[q.currentDefinitionListIndex];
	}
	else
	{
		q.currentDefinitionListIndex = 0;
		return q.definitionList[q.currentDefinitionListIndex];
	}
}

FString QuestionManager::GetPrevDef(Question &q)
{
	if (q.currentDefinitionListIndex < 0)
	{
		q.currentDefinitionListIndex -= 1;
		return q.definitionList[q.currentDefinitionListIndex];
	}
	else
	{
		q.currentDefinitionListIndex = q.definitionList.Num() - 1;
		return q.definitionList[q.currentDefinitionListIndex];
	}
}

int QuestionManager::WordTypeParser(std::string wordTypeString)
{
	// 0 noun
	// 1 adj
	// 2 verb
	// 3 other
	for (int i = 0; i < wordTypeString.length(); i++)
	{
		if (wordTypeString[i] == 'a')
		{
			if (i + 1 < wordTypeString.length())
			{
				if (wordTypeString[i + 1] == 'd')
				{
					if (i + 2 < wordTypeString.length())
					{
						if (wordTypeString[i + 2] == 'j')
						{
							return 1;
						}
						else if (wordTypeString[i + 2] == 'v')
						{
							return 1;
						}
					}
				}
			}
		}
		else if (wordTypeString[i] == 'n')
		{
			if (i == 0 && wordTypeString.length() == 1)
			{
				return 0;
			}
			else if (i + 1 < wordTypeString.length())
			{
				if (wordTypeString[i + 1] == 'u')
				{
					return 0;
				}
			}
			else if (wordTypeString[i + 1] == ',')
			{
				if (i + 2 < wordTypeString.length())
				{
					if (wordTypeString[i + 2] == 'v')
					{
						return 0;
					}
				}
			}
		}
		else if (wordTypeString[i] == 'v')
		{
			return 2;
		}
		else if (wordTypeString[i] == 's')
		{
			if (i + 1 < wordTypeString.length())

			{
				if (wordTypeString[i + 1] == 'u')
				{
					if (i + 2 < wordTypeString.length())
					{
						if (wordTypeString[i + 2] == 'f')
						{
							return 0;
						}
					}
				}
			}
		}
	}
	return 3;
}

Question QuestionManager::GetCurrentQuestion()
{
	return currentQuestion;
}

void QuestionManager::GetNewQuestion()
{
	if (currentQuestionIndex != -1)
	{
		std::vector<Question> tempVector;

		for (int i = 0; i < questionVector.size(); i++)
		{
			if (i != currentQuestionIndex)
			{
				tempVector.push_back(questionVector.at(i));
			}
		}

		questionVector = tempVector;
	}

	int randomIndex = FMath::RandRange(0, questionVector.size() - 1);
	currentQuestion = questionVector.at(randomIndex);

	currentQuestionIndex = randomIndex;
	GetDummyAnswers(randomIndex);
	SetRandomCorrectAnswerIndex();
	answerVector.clear();

	for (int i = 0; i < 5; i++)
	{
		if (i == currentQuestionCorrectAnswerIndex)
		{
			answerVector.push_back(currentQuestion);
		}
		else
		{
			if (i < 4)
			{
				answerVector.push_back(dummyAnswerVector.at(i));
			}
			else
			{
				answerVector.push_back(dummyAnswerVector.at(3));
			}
		}
	}
}

void QuestionManager::GetNewQuestion2()
{
	if (currentQuestionIndex == -1)
	{
		std::vector<Question> tempVector;

		for (int i = startIndex; i < startIndex + totalQuestionsToRead; i++)
		{
			tempVector.push_back(questionVector.at(i));
		}

		questionVector = tempVector;
	}

	if (lastQuestionAnsweredCorrectly)
	{
		if (currentQuestionIndex != -1)
		{
			std::vector<Question> tempVector;

			for (int i = 0; i < questionVector.size(); i++)
			{
				if (i != currentQuestionIndex)
				{
					tempVector.push_back(questionVector.at(i));
				}
			}

			questionVector = tempVector;
		}
	}

	int randomIndex = rand() % ((questionVector.size()));
	currentQuestion = questionVector.at(randomIndex);
	currentQuestionIndex = randomIndex;

	GetDummyAnswers2();
	SetRandomCorrectAnswerIndex();
	answerVector.clear();
	answerVector.shrink_to_fit();
	int dummyIndexAdded = 0;
	for (int i = 0; i < 5; i++)
	{
		if (i == currentQuestionCorrectAnswerIndex)
		{
			answerVector.push_back(currentQuestion);
		}
		else
		{
			answerVector.push_back(dummyAnswerVector.at(dummyIndexAdded));
			dummyIndexAdded++;
		}
	}
}

void QuestionManager::GetDummyAnswers(int index)
{
	dummyAnswerVector.clear();
	usedDummyIndexes.clear();
	for (int i = 0; i < 4; i++)
	{
		int randomIndex = FMath::RandRange(0, questionVector.size() - 1);
		while (randomIndex == index || CheckUsedDummyIndexes(randomIndex))
		{
			randomIndex = FMath::RandRange(0, questionVector.size() - 1);
		}
		dummyAnswerVector.push_back(questionVector.at(randomIndex));
		usedDummyIndexes.push_back(randomIndex);
	}
}

void QuestionManager::GetDummyAnswers2()
{
	dummyAnswerVector.clear();
	dummyAnswerVector.shrink_to_fit();
	usedDummyIndexes.clear();
	usedDummyIndexes.shrink_to_fit();

	switch (wordTypeMode)
	{
	case -1:
		for (int i = 0; i < 4; i++)
		{
			int dummyRandomIndex;
			switch (currentQuestion.wordTypeCode)
			{
			case 0:
				dummyRandomIndex = FMath::RandRange(0, nounsVector.size() - 1);
				while (CompareStringContents(nounsVector.at(dummyRandomIndex).english,questionVector.at(currentQuestionIndex).english) || CheckUsedDummyIndexes2(dummyRandomIndex))
				{
					dummyRandomIndex = FMath::RandRange(0, nounsVector.size() - 1);
				}
				dummyAnswerVector.push_back(nounsVector.at(dummyRandomIndex));
				usedDummyIndexes.push_back(dummyRandomIndex);
				break;
			case 1:
				dummyRandomIndex = FMath::RandRange(0, adjsVector.size() - 1);
				while (CompareStringContents(adjsVector.at(dummyRandomIndex).english,questionVector.at(currentQuestionIndex).english) || CheckUsedDummyIndexes2(dummyRandomIndex))
				{
					dummyRandomIndex = FMath::RandRange(0, adjsVector.size() - 1);
				}
				dummyAnswerVector.push_back(adjsVector.at(dummyRandomIndex));
				usedDummyIndexes.push_back(dummyRandomIndex);
				break;
			case 2:
				dummyRandomIndex = FMath::RandRange(0, verbsVector.size() - 1);
				while (CompareStringContents(verbsVector.at(dummyRandomIndex).english,questionVector.at(currentQuestionIndex).english) || CheckUsedDummyIndexes2(dummyRandomIndex))
				{
					dummyRandomIndex = FMath::RandRange(0, verbsVector.size() - 1);
				}
				dummyAnswerVector.push_back(verbsVector.at(dummyRandomIndex));
				usedDummyIndexes.push_back(dummyRandomIndex);
				break;
			default:
				for (int i = 0; i < 4; i++)
				{
					int dummyRandomIndex = FMath::RandRange(0, totalResourceVector.size() - 1);
					while (totalResourceVector.at(dummyRandomIndex).english == questionVector.at(currentQuestionIndex).english || CheckUsedDummyIndexes2(dummyRandomIndex))
					{
						dummyRandomIndex = FMath::RandRange(0, totalResourceVector.size() - 1);
					}
					dummyAnswerVector.push_back(totalResourceVector.at(dummyRandomIndex));
					usedDummyIndexes.push_back(dummyRandomIndex);
				}
				break;
			}
		}
		break;
	default:
		for (int i = 0; i < 4; i++)
		{
			int dummyRandomIndex = FMath::RandRange(0, totalResourceVector.size() - 1);
			while (totalResourceVector.at(dummyRandomIndex).english == questionVector.at(currentQuestionIndex).english || CheckUsedDummyIndexes2(dummyRandomIndex))
			{
				dummyRandomIndex = FMath::RandRange(0, totalResourceVector.size() - 1);
			}
			dummyAnswerVector.push_back(totalResourceVector.at(dummyRandomIndex));
			usedDummyIndexes.push_back(dummyRandomIndex);
		}
		break;
	}
}

bool QuestionManager::CompareStringContents(FString stringA, FString stringB)
{
	TArray<FString> stringAWordArray;
	FString tempString = "";

	for (int i = 0; i < stringA.Len(); i++)
	{
		if (stringA[i] != ' ')
		{
			tempString += stringA[i];
		}
		else
		{
			tempString = tempString.TrimStart();
			tempString = tempString.TrimEnd();
			tempString = " " + tempString + " ";
			stringAWordArray.Add(tempString);
			tempString = "";
		}
	}

	for (int i = 0; i < stringAWordArray.Num(); i++)
	{
		if (stringB.Contains(stringAWordArray[i]) && stringAWordArray[i] != " a " && stringAWordArray[i] != " the " && stringAWordArray[i] != " in " && stringAWordArray[i] != " of ")
		{
			return true;
		}
	}

	return false;
}

bool QuestionManager::CheckUsedDummyIndexes2(int index)
{
	if (usedDummyIndexes.size() == 0)
	{
		return false;
	}
	for (int i = 0; i < usedDummyIndexes.size(); i++)
	{
		if (usedDummyIndexes.at(i) == index)
		{
			return true;
		}
	}
	return false;
}

bool QuestionManager::CheckUsedDummyIndexes(int index)
{
	bool used = false;
	for (int i = 0; i < usedDummyIndexes.size(); i++)
	{
		if (usedDummyIndexes.at(i) == index)
		{
			used = true;
			break;
		}
	}
	return used;
}

void QuestionManager::SetRandomCorrectAnswerIndex()
{
	currentQuestionCorrectAnswerIndex = FMath::RandRange(0, 4);
}

int QuestionManager::GetQuestionMode()
{
	return questionMode;
}

int QuestionManager::GetCorrectIndex()
{
	return currentQuestionCorrectAnswerIndex;
}

int QuestionManager::GetCurrentQuestionIndex()
{
	return currentQuestionIndex;
}

void QuestionManager::IncrementQuestionsAnsweredCorrectly()
{
	numberOfQuestionsAnsweredCorreclty += 1;
}

int QuestionManager::GetNumberOfQuestionsAnsweredCorrectly()
{
	return numberOfQuestionsAnsweredCorreclty;
}

int QuestionManager::GetTotalNumberOfQuestions()
{
	return totalQuestions;
}
