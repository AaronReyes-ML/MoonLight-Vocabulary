// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "CSV_Question.generated.h"


USTRUCT(BlueprintType)
struct FQData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Question)
		FString hiragana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Question)
		FString kanji;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Question)
		FString wordtype;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Question)
		FString english;
};

/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API UCSV_Question : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
	
	
};
