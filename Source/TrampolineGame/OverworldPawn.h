// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OverworldPawn.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AOverworldPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOverworldPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetInitialCharacterLocation();
	void UpdateCompletedLevelDetails();

	UFUNCTION(BlueprintCallable)
	void DoCommitFromUI();

	class UTrampolineGameInstance* gameInstance;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* characterMesh;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* cameraComponent;
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* springArmComponent;

	UPROPERTY(EditAnywhere)
	class USplineComponent* currentSpline;

	void SetCurrentSpline(USplineComponent* sp, int ID);

	UPROPERTY(EditAnywhere)
	class AJunction* startJunction;
	UPROPERTY(EditAnywhere)
	class AJunction* endJunction;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* overlapComponent;

	UPROPERTY(EditAnywhere)
	float moveAmount = 10.f;

	float upInput = 0;
	float rightInput = 0;
	float upInputButton = 0;
	float rightInputButton = 0;


	void MoveUp(float inputValue);
	void MoveRight(float inputValue);

	float GetDistanceTraveledOnSpline(USplineComponent* spline);
	float distanceTraveledOnCurrentSpline = 0.f;

	void DoMovement(FVector &splineTangentDirection, FVector &desiredMoveDirection);
	bool lastMovePositive = true;
	bool isOverrun = false;
	bool overrunOnPositive = false;
	bool isOverrunReturning = false;
	bool doingMove = false;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	class AJunction* currentJunction;

	bool isInJunction = false;

	bool junctionHasUp = false;
	bool junctionHasLeft = false;
	bool junctionHasDown = false;
	bool junctionHasRight = false;

	bool junctionEntranceUp = false;
	bool junctionEntranceLeft = false;
	bool junctionEntranceDown = false;
	bool junctionEntranceRight = false;

	class USplineComponent* junctionUpSpline;
	class USplineComponent* junctionLeftSpline;
	class USplineComponent* junctionDownSpline;
	class USplineComponent* junctionRightSpline;

	class USplineComponent* junctionTargetSpline;
	bool isInterpolatingToJunctionSpline = false;
	bool targetIsEntranceSpline = false;

	void DoInterpolateToSpline(USplineComponent* target);

	class ALevelPrompt* currentLevelPrompt;

	bool isLevelPrompted = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UIWidgetBase;

	class UTrampolineOverworldWidget* UIWidget;

	void UpdateGameInstanceForCampaignLevel(int fCode, int sIndex, int toRead, int qWin, int qLose, int wMode, int qMode);

	void RefreshAllLevelPrompts();

	UPROPERTY(EditAnywhere)
	int numberOfLevelsOnOverworld = 0;

	int levelID = 0;

	bool isInStarGate = false;
	bool isStarGateBlocked = false;

	class AItemPickup* currentItemPickup;
	bool isInItemPickup = false;
	void DoItemPickup();
	void RefreshAllItemPickups();

	void RefreshAllStarGates();

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* lunaPet;
	UPROPERTY(EditAnywhere)
	class UWolfAnimInstance* lunaPetAnimInstance;

	UPROPERTY(EditAnywhere)
	class UPointLightComponent* lunaPetLight;

	bool hasAcknowledgedLevelImage = false;

	UPROPERTY(EditAnywhere)
	class ALevelTransitionActor* forwardLevelTransitionActor;

	void DoStarGateUnblock(int starGateID);

	UParticleSystem* fogClearedParticleSystem;
	class UParticleSystemComponent* fogClearedParticleSystemComponent;

	FString localSaveSlotName = TEXT("SaveSlot1");

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void DoCommitToLevel();

	int totalStarCount = 0;
	int totalAutoFeverHeld = 0;
	int totalQuestionReduceHeld = 0;
	int totalSupportHeld = 0;
	int totalBossKeys = 0;

	UFUNCTION(BlueprintCallable)
	void CommitToGameInstance();

	UFUNCTION(BlueprintCallable)
	void SaveAndReturnToMenu();

	void TransitionToLevel(int levelID, int previousLevelID);

	UFUNCTION(BlueprintCallable)
	void GetLevelID();

	UFUNCTION(BlueprintCallable)
	bool GetCameFromOverworldLevel();

	UFUNCTION(BlueprintCallable)
	void SetUpInput(float input);

	UFUNCTION(BlueprintCallable)
	void SetRightInput(float input);
};
