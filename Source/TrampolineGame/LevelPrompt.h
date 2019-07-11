// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelPrompt.generated.h"

UCLASS()
class TRAMPOLINEGAME_API ALevelPrompt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelPrompt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USceneComponent* root;
	UPROPERTY(EditAnywhere)
	class USphereComponent* levelPromptOverlap;

	UPROPERTY(EditAnywhere)
	FString levelName = "";
	UPROPERTY(EditAnywhere)
	FString levelText = "";

	UPROPERTY(EditAnywhere)
	class USpotLightComponent* levelPromptSpotLight;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* levelPromptLightShaft;

	UParticleSystem* levelPromptParticleSystem;
	class UParticleSystemComponent* levelPromptParticleSystemComponent;

	UPROPERTY(EditAnywhere)
	bool isBossLevel = false;
	UPROPERTY(EditAnywhere)
	int bossKeyReward = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	int levelID = 0;

	UPROPERTY(EditAnywhere)
	int fileCode = -1;
	UPROPERTY(EditAnywhere)
	int startIndex = -1;
	UPROPERTY(EditAnywhere)
	int totalQuestionsToRead = -1;
	UPROPERTY(EditAnywhere)
	int minQuestionsToWin = -1;
	UPROPERTY(EditAnywhere)
	int minQuestionsToLose = -1;
	UPROPERTY(EditAnywhere)
	int wordTypeMode = -1;
	UPROPERTY(EditAnywhere)
	int questionMode = -1;
	int scoreThresholdForClear = -1;
	int scoreThresholdFor2Star = -1;
	int scoreThresholdFor3Star = -1;

	UPROPERTY(EditAnywhere)
	bool hasBeenCompleted = false;
	UPROPERTY(EditAnywhere)
	int completedScore = -1;
	UPROPERTY(EditAnywhere)
	int completedQuestionsCorrect = -1;
	UPROPERTY(EditAnywhere)
	int starsEarned = -1;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	FString GetLevelName() { return levelName; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	FString GetLevelText() { return levelText; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetWordTypeMode() { return wordTypeMode; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetLevelID() { return levelID; };


	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetQuestionMode() { return questionMode; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetQuestionsToWin() { return minQuestionsToWin; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetTotalQuestionsToRead() { return totalQuestionsToRead; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetFileCode() { return fileCode; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	bool GetCompleted() { return hasBeenCompleted; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetScore() { return completedScore; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetQuestionsCorrect() { return completedQuestionsCorrect; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetStars() { return starsEarned; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE	bool GetIsBossLevel() { return isBossLevel; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE	int GetBossKeyReward() { return bossKeyReward; };

	void DoPlayLevelPromptParticles();
	void StopLevelPromptParticles();

	bool isPlayingParticles = false;
};
