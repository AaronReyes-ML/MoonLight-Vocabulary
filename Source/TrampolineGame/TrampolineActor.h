// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestionManager.h"
#include "TrampolineGameWidget.h"
#include "PaperCharacter.h"
#include "TrampolineActor.generated.h"

/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API ATrampolineActor : public APaperCharacter
{
	GENERATED_BODY()
	
	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* actorOverlapComponent;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

protected:
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* InAirAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere)
	class AQuestionActor* qActor;

	int jumpState = -1;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Slection
	void SelectOne();
	void SelectTwo();
	void SelectThree();
	void SelectFour();
	void SelectFive();
	bool canMakeSelection = true;
	void HandleSelection(int index);
	bool answeredWrong = false;
	bool acknowledgedCorrectAnswer = false;
	void EnterAcknowledgeMode();
	void ExitAcknowledgeMode();

	// Camera
	UPROPERTY(EditAnywhere)
	float defaultOrthoWidth = 2500.f;
	UPROPERTY(EditAnywhere)
	float extendedOrthoWidth = 1500.f;

	// Flow
	void Start();
	bool hasStarted = false;

	// Movement
	UFUNCTION(BlueprintCallable)
	void JumpFromUI();
	void Trampoline();
	UPROPERTY(EditAnywhere)
	float currentImpulseForce = 45000.f;
	float perfectCenterLocation = -50;
	UPROPERTY(EditAnywhere)
	float currentGravityMultiplier = .35f;
	int jumpCooldown = 30;
	int maxJumpLevel = 50;

	// Camera Operationm
	void SetCameraOrthoWidth(float dTime);
	float defaultCameraOrthoWidth = 2500.f;
	float maxCameraOrthoWidth = 4500.f;
	float cameraMinDistance = 900;
	float cameraMaxDistance = 1500;
	bool doingInitCameraZoomout = false;
	bool initCameraZoomoutComplete = false;
	void SetCameraBoomLength(float dTime, bool init);

	// Character status
	int score = 0;
	int singleQuestionScoreReward = 50;

	bool gameOver = false;
	void GameOver();
	bool gameWon = false;
	void GameWon();
	bool gamePaused = false;
	void GamePause();
	UFUNCTION(BlueprintCallable)
	void GamePauseFromUI();
	void UpdateCharacter(float multiplier);
	float health = 1.f;
	void UpdateHealth(float amount);

	float progressMeter = 0.5;
	float progressAmountToAdd = 0;
	float progressAmountToRemove = 0;
	float progressMeterIncreaseAmount = .05;
	float singleQuestionReward = .20;
	float nonMultiplierQuestionsCorrect = 0;
	void UpdateProgressMeter(bool negative);
	void InterpProgressMeter();

	bool isBuildingMultiplier = true;
	int subsequentCorrectQuestions = 0;
	int subsequentIncorrectQuestions = 0;
	int subsequentIncorrectQuestionsToKillMutliplier = 1;
	int subsequentIncorrectQuestionsToDoDamage = 2;
	float currentComboMultiplier = 1.f;
	void UpdateComboMultiplier(int increment);

	bool isBuildingFever = false;
	bool feverReady = false;
	bool isInFever = false;
	float maxFeverTimeRemaining = 800.f;
	float feverTimeRemaining = 0.f;
	float feverSingleQuestionReward = 0.03;
	float feverBaseReward = .25;
	float feverExtendReward = 400.f;
	int feverExtendRewardCounter = 0;
	int feverSubsequentCorrectQuestions = 0;
	int subsequentIncorrectQuestionsToExitFever = 3;
	int feverQuestionsCorrectForReward = 5;
	float feverMeter = 0.0;
	void EnterFever();
	void ExitFever();
	void DoEnterFever();
	void UpdateFeverMeter();
	UFUNCTION(BlueprintCallable)
	void EnterFeverFromUI();

	bool isBuildingTranscendance = false;
	bool isInTranscendance = false;
	float transcendanceBuildReward = .3;
	float transcendanceMeter = 0.0;
	int questionsCorrectToBuildTranscendance = 10;
	void EnterTranscendance();
	void ExitTranscendance();
	void DoEnterTranscendance();
	void UpdateTranscendanceMeter();

	// Jump details
	void UpdateJumpLevel(float newJumpLevel);
	float currentJumpLevel = 1.f;
	float initialJumpPower = 45000;
	float jumpPowerPerLevelMultiplier = .2;

	bool extendingHeight = false;
	float currentJumpLevelMaxHeight = 1000.f;

	// Question Details
	QuestionManager questionManager;
	std::vector<class Question> historyVector;
	void AssignNewQuestion();
	UFUNCTION(BlueprintCallable)
	void GetNextDefinition();
	UFUNCTION(BlueprintCallable)
	void GetPreviousDefinition();
	void UpdateHistory(Question &questionToAdd);
	FString GetFirstDef(const FString &def);
	// What value is being used to display the question
	// 0 for primary mode (see QuestionManager) 1 for secondary mode
	int questionDisplayMode = 0;
	bool isShowingDetailedHistory = false;

	// UI
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UIWidgetBase;
	class UTrampolineGameWidget* UIWidget;

	class UTrampolineGameInstance* gameInstance;

	void HandleLaunch();
	bool launchDone = false;
	bool isSettingLaunchMeter = false;
	bool launchMeterVisible = false;
	void StartLaunchMeter();
	void DoLaunch();
	void CommitMeter();
	bool initPress = false;
	float launchMeterFirstPressPercent = 0;
	bool launchMeterFirstPressCommited = false;
	float launchMeterSecondPressPercent = 0;
	bool launchMeterSecondPressCommited = false;
	bool launchMeterFillingUp = true;

	void UpdateScore();

	int currentPowerup = 0;
	int autoFeverHeld = 0;
	int questionReduceHeld = 0;
	int questionReduceCharges = 0;
	int supportHeld = 0;
	int supportIndex = 0;
	bool isSupportPaused = false;
	void DoSupportPause();
	UFUNCTION(BlueprintCallable)
	void DoSupportResume();
	void DoSupport1();
	void DoSupport2();
	void DoSupport3();
	void DoSupport4();
	void DoSupport5();
	void DoAbilityFever();
	void DoAbilityReduce();
	void DoQuestionReduceHighlight(bool support);
	bool questionReduceHighlightActive = false;

	UParticleSystem* correctParticleSystem;
	UParticleSystem* incorrectParticleSystem;
	class UParticleSystemComponent* answerParticleSystemComponent;

	UParticleSystem* jumpParticleSystem;
	UParticleSystem* groundParticleSystem;
	class UParticleSystemComponent* jumpParticleSystemComponent;

	UParticleSystem* feverActivatedParticleSystem;
	UParticleSystem* transcendanceActivatedParticleSystem;
	class UParticleSystemComponent* feverActivatedParticleSystemComponent;

	void PlayAnswerParticleSystem(UParticleSystem* pSystem);
	void PlayJumpParticleSystem(UParticleSystem* pSystem);
	void PlayFeverActivatedParticleSystem(UParticleSystem* pSystem);

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* lunaPet;
	UPROPERTY(EditAnywhere)
	class UWolfAnimInstance* lunaPetAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* N5DataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* N4DataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* N3DataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* N2DataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* N1DataTable;

	FString localSaveSlotName = "SaveSlot1";

public:
	ATrampolineActor();

	UFUNCTION(BlueprintCallable)
	void UpdateLevelDetails();
	UFUNCTION(BlueprintCallable)
	void Save();
	UFUNCTION(BlueprintCallable)
	float GetHealth();
	UFUNCTION(BlueprintCallable)
	float GetProgress();
	UFUNCTION(BlueprintCallable)
	float GetCurrentComboMult();
	UFUNCTION(BlueprintCallable)
	float GetCurrentJumpLevelMaxHeight();
	UFUNCTION(BlueprintCallable)
	float GetCurrentHeight();
	UFUNCTION(BlueprintCallable)
	float GetAltitudeIndicatorOffset();
	UFUNCTION(BlueprintCallable)
	float GetCurrentJumpLevel();
	UFUNCTION(BlueprintCallable)
	void SelectionMadeFromUI(int index);
	UFUNCTION(BlueprintCallable)
	void ToggleQuestionDisplay();
	UFUNCTION(BlueprintCallable)
	void DetailedHistoryDisplay();
	UFUNCTION(BlueprintCallable)
	void DetailedHistoryHide();
	UFUNCTION(BlueprintCallable)
	FString GetHistoryKanji(int index);
	UFUNCTION(BlueprintCallable)
	FString GetHistoryHiragana(int index);
	UFUNCTION(BlueprintCallable)
	FString GetHistoryEnglish(int index);
	UFUNCTION(BlueprintCallable)
	void SetActivePowerup(int powerup);
	UFUNCTION(BlueprintCallable)
	void DoPowerup();
	UFUNCTION(BlueprintCallable)
	int GetLevelID();
};
