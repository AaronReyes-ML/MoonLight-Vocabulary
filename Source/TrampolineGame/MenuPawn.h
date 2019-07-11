// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <string>
#include "MenuPawn.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AMenuPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMenuPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* camera;
	
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UIWidgetBase;
	class UTrampolineMenuWidget* UIWidget;

	class UTrampolineGameInstance* gameInstance;

	UPROPERTY(EditAnywhere)
	int totalLevelsOnStage1 = 0;
	UPROPERTY(EditAnywhere)
	int totalItemPickupsOnStage1 = 0;
	UPROPERTY(EditAnywhere)
	int totalStarGatesOnStage1 = 0;

	UPROPERTY(EditAnywhere)
	int initalStars = 0;
	UPROPERTY(EditAnywhere)
	int initialBossKeys = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void UpdateGameInstanceForFreePlay(int fCode, int wordTypeMode, int questionMode);

	UFUNCTION(BlueprintCallable)
	void CreateGameSave(int index);

	UFUNCTION(BlueprintCallable)
	bool DoesGameSaveExist();

	UFUNCTION(BlueprintCallable)
	int GetLevelID(int index);

	UFUNCTION(BlueprintCallable)
	int GetSaveSlot1Progress();

	UFUNCTION(BlueprintCallable)
	int GetSaveSlot2Progress();

	UFUNCTION(BlueprintCallable)
	int GetSaveSlot3Progress();

	UFUNCTION(BlueprintCallable)
	void SetGameInstanceSaveSlotName(FString fileName);
};
