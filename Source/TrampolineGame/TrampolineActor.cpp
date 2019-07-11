// Fill out your copyright notice in the Description page of Project Settings.

#include "TrampolineActor.h"
#include "PaperFlipbookComponent.h"
#include "Question.h"
#include "QuestionActor.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "TrampolineGameInstance.h"
#include "TrampolineSaveGame.h"
#include "Particles/ParticleSystemComponent.h"
#include "Classes/Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "WolfAnimInstance.h"
#include "Engine.h"
#include "Engine/DataTable.h"
#include <ctime>

#pragma region setup/start

ATrampolineActor::ATrampolineActor()
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1250.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->bAbsoluteRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->RelativeRotation = FRotator(0.0f, -90.0f, -10.0f);

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Perspective;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->bAbsoluteRotation = true;
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	actorOverlapComponent = CreateDefaultSubobject<UBoxComponent>("Actor overlap component");

	correctParticleSystem = CreateDefaultSubobject<UParticleSystem>("Correct particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> pSO0(TEXT("/Game/Trampoline/Particles/Q_Correct_ParticleSystem"));
	correctParticleSystem = pSO0.Object;

	incorrectParticleSystem = CreateAbstractDefaultSubobject<UParticleSystem>("Incorrect particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> pSO1(TEXT("/Game/Trampoline/Particles/Q_Wrong_ParticleSystem"));
	incorrectParticleSystem = pSO1.Object;

	jumpParticleSystem = CreateDefaultSubobject<UParticleSystem>("Jump particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> pSO2(TEXT("/Game/Trampoline/Particles/MagicCircles/MCircle_Jump_Good"));
	jumpParticleSystem = pSO2.Object;

	groundParticleSystem = CreateDefaultSubobject<UParticleSystem>("Ground particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> pSO3(TEXT("/Game/Trampoline/Particles/MagicCircles/MCircle_Jump_Bad"));
	groundParticleSystem = pSO3.Object;

	feverActivatedParticleSystem = CreateDefaultSubobject<UParticleSystem>("Fever activated particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> pSO4(TEXT("/Game/Trampoline/Particles/MagicCircles/MCircle_FeverActivated"));
	feverActivatedParticleSystem = pSO4.Object;

	transcendanceActivatedParticleSystem = CreateDefaultSubobject<UParticleSystem>("Transendance activated particleSystem");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> psO5(TEXT("/Game/Trampoline/Particles/MagicCircles/MCircle_TranscendanceActivated"));
	transcendanceActivatedParticleSystem = psO5.Object;

	lunaPet = CreateDefaultSubobject<USkeletalMeshComponent>("Luna's pet");
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skelMeshObj0(TEXT("/Game/ParagonShinbi/Characters/Heroes/Shinbi/Shinbi_Wolf/Meshes/Shinbi_Wolf"));
	lunaPet->SetupAttachment(GetCapsuleComponent());
	lunaPet->SetSkeletalMesh(skelMeshObj0.Object);
	lunaPet->SetRelativeLocation(FVector::ZeroVector);
	lunaPet->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	lunaPet->AddLocalOffset(FVector(0, 0, -20));
	lunaPet->AddLocalRotation(FRotator(0,25,0));
	
	static ConstructorHelpers::FClassFinder<UObject> animBPClassObj0(TEXT("/Game/ParagonShinbi/Characters/Heroes/Shinbi/Shinbi_Wolf/Animations/Wolf_TrampolineAnimInstance_BP"));
	lunaPet->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	lunaPet->SetAnimInstanceClass(animBPClassObj0.Class);

	N5DataTable = CreateDefaultSubobject<UDataTable>("N5 table");
	static ConstructorHelpers::FObjectFinder<UDataTable> dataTableAsset0(TEXT("/Game/Trampoline/Lists/N5"));
	N5DataTable = dataTableAsset0.Object;

	N4DataTable = CreateDefaultSubobject<UDataTable>("N4 table");
	static ConstructorHelpers::FObjectFinder<UDataTable> dataTableAsset1(TEXT("/Game/Trampoline/Lists/N4"));
	N4DataTable = dataTableAsset1.Object;

	N3DataTable = CreateDefaultSubobject<UDataTable>("N3 table");
	static ConstructorHelpers::FObjectFinder<UDataTable> dataTableAsset2(TEXT("/Game/Trampoline/Lists/N3"));
	N3DataTable = dataTableAsset2.Object;

	N2DataTable = CreateDefaultSubobject<UDataTable>("N2 table");
	static ConstructorHelpers::FObjectFinder<UDataTable> dataTableAsset3(TEXT("/Game/Trampoline/Lists/N2"));
	N2DataTable = dataTableAsset3.Object;

	N1DataTable = CreateDefaultSubobject<UDataTable>("N1 table");
	static ConstructorHelpers::FObjectFinder<UDataTable> dataTableAsset4(TEXT("/Game/Trampoline/Lists/N1"));
	N1DataTable = dataTableAsset4.Object;
}

void ATrampolineActor::BeginPlay()
{
	Super::BeginPlay();

	srand(time(NULL));

	GetCharacterMovement()->GravityScale = 0.f;
	gameInstance = Cast<UTrampolineGameInstance>(GetGameInstance());

	if (gameInstance)
	{
		localSaveSlotName = gameInstance->instancedSaveSlotName;
	}

	if (lunaPet)
	{
		lunaPetAnimInstance = Cast<UWolfAnimInstance>(lunaPet->GetAnimInstance());
	}

	UIWidget = Cast<UTrampolineGameWidget>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	if (UIWidget)
	{
		UIWidget->AddToViewport();
	}

	questionManager = QuestionManager();

	// Establish initial values
	if (gameInstance->isFreePlay)
	{
		// Setup question manager
		questionManager.isFreePlay = true;
		questionManager.fileCode = gameInstance->fileCode;
		questionManager.startIndex = 0;
		questionManager.totalQuestionsToRead = -1;
		questionManager.minQuestionsToWin = -1;
		questionManager.wordTypeMode = gameInstance->wordTypeMode;
		questionManager.questionMode = gameInstance->questionMode;

		// Load in questions
		questionManager.FileParser2(N5DataTable);
		UIWidget->UpdateTotalQuestionsUI(0, questionManager.minQuestionsToWin, questionManager.totalQuestionsToRead);
		UIWidget->UpdateScoreUI(0);
		UIWidget->UpdateComboUI(1, 0);
	}
	else
	{
		gameInstance->cameFromLevel = true;
		// Setup question manager
		questionManager.isFreePlay = false;
		questionManager.fileCode = gameInstance->fileCode;
		questionManager.startIndex = gameInstance->startIndex;
		questionManager.totalQuestionsToRead = gameInstance->totalQuestionsToRead;
		questionManager.minQuestionsToWin = gameInstance->minQuestionsToWin;
		questionManager.wordTypeMode = gameInstance->wordTypeMode;
		questionManager.questionMode = gameInstance->questionMode;
		autoFeverHeld = gameInstance->autoFeverHeld;
		questionReduceHeld = gameInstance->questionReduceHeld;
		supportHeld = gameInstance->supportHeld;

		// Load in questions
		switch (questionManager.fileCode)
		{
		case 5:
			questionManager.FileParser2(N5DataTable);
			break;
		case 4:
			questionManager.FileParser2(N4DataTable);
			break;
		case 3:
			questionManager.FileParser2(N3DataTable);
			break;
		case 2:
			questionManager.FileParser2(N2DataTable);
			break;
		case 1:
			questionManager.FileParser2(N1DataTable);
			break;
		default:
			break;
		}

		UIWidget->UpdateTotalQuestionsUI(0, questionManager.minQuestionsToWin, questionManager.totalQuestionsToRead);
		UIWidget->UpdatePowerupUI(autoFeverHeld, questionReduceHeld, supportHeld);

		gameInstance->scoreThresholdForClear = questionManager.minQuestionsToWin * singleQuestionScoreReward;
		gameInstance->scoreThresholdFor3Star = questionManager.totalQuestionsToRead * 3 * singleQuestionScoreReward;
		gameInstance->scoreThresholdFor2Star = FMath::TruncToInt((gameInstance->scoreThresholdFor3Star - gameInstance->scoreThresholdForClear) / 2) + gameInstance->scoreThresholdForClear;

		currentJumpLevel = 3;

		UIWidget->UpdateStarTargets(gameInstance->scoreThresholdForClear, gameInstance->scoreThresholdFor2Star, gameInstance->scoreThresholdFor3Star);
		UIWidget->UpdateScoreUI(0);
		UIWidget->UpdateComboUI(1, 0);
		UIWidget->UpdateJumpLevelUI(currentJumpLevel);
	}

	UIWidget->DisableAllGameplayButtons();

	APlayerController* menuController = GetWorld()->GetFirstPlayerController();
	menuController->bShowMouseCursor = true;
	menuController->bEnableClickEvents = true;
	menuController->bEnableMouseOverEvents = true;

	actorOverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ATrampolineActor::OnOverlapBegin);
	actorOverlapComponent->OnComponentEndOverlap.AddDynamic(this, &ATrampolineActor::OnOverlapEnd);
}

void ATrampolineActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!hasStarted && isSettingLaunchMeter && !launchDone)
	{
		HandleLaunch();
	}

	if (doingInitCameraZoomout)
	{
		SetCameraBoomLength(DeltaSeconds, true);
	}

	//GEngine->AddOnScreenDebugMessage(1, 20, FColor::White, FString::SanitizeFloat(currentImpulseForce)); FScreenMessageString* mForce = GEngine->ScreenMessages.Find(1); mForce->TextScale.X = mForce->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(2, 20, FColor::White, FString::SanitizeFloat(currentJumpLevelMaxHeight)); FScreenMessageString* mJumpHeight = GEngine->ScreenMessages.Find(2); mJumpHeight->TextScale.X = mJumpHeight->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(4, 20, FColor::White, questionManager.questionVector.at(0).kanji.c_str()); FScreenMessageString* mKanji = GEngine->ScreenMessages.Find(4); mKanji->TextScale.X = mKanji->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(5, 20, FColor::White, questionManager.questionVector.at(0).hiragana.c_str()); FScreenMessageString* mHiragana = GEngine->ScreenMessages.Find(5); mHiragana->TextScale.X = mHiragana->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(6, 20, FColor::White, questionManager.questionVector.at(0).english.c_str()); FScreenMessageString* mEnglish = GEngine->ScreenMessages.Find(6); mEnglish->TextScale.X = mEnglish->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(7, 20, FColor::White, FString::SanitizeFloat(health)); FScreenMessageString* mHealth = GEngine->ScreenMessages.Find(7); mHealth->TextScale.X = mHealth->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(8, 20, FColor::White, FString::SanitizeFloat(progressMeter)); FScreenMessageString* mProg = GEngine->ScreenMessages.Find(8); mProg->TextScale.X = mProg->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(9, 20, FColor::White, FString::SanitizeFloat(GetAltitudeIndicatorOffset())); FScreenMessageString* mPos = GEngine->ScreenMessages.Find(9); mPos->TextScale.X = mPos->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(10, 20, FColor::White, FString::FromInt(feverSubsequentCorrectQuestions)); FScreenMessageString* mFeverQuestions = GEngine->ScreenMessages.Find(10); mFeverQuestions->TextScale.X = mFeverQuestions->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(11, 20, FColor::White, FString("Jump state") + FString::FromInt(jumpState)); FScreenMessageString* mJumpState = GEngine->ScreenMessages.Find(11); mJumpState->TextScale.X = mJumpState->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(12, 20, FColor::White, FString("Jump cooldown") + FString::FromInt(jumpCooldown)); FScreenMessageString* mJumpCooldown = GEngine->ScreenMessages.Find(12); mJumpCooldown->TextScale.X = mJumpCooldown->TextScale.Y = 1.0f;

	if (hasStarted && launchDone)
	{

		float zPos = actorOverlapComponent->GetComponentLocation().Z;

		if (zPos > currentJumpLevelMaxHeight)
		{
			currentJumpLevelMaxHeight = zPos;
		}

		//UIWidget->UpdateAltitudeOffsetUI((zPos / currentJumpLevelMaxHeight) * -767, extendingHeight, (int)floorf(zPos), (int)floorf(currentJumpLevelMaxHeight));

		// Zoom camera based on movement
		//SetCameraOrthoWidth(DeltaSeconds);

		if (progressAmountToAdd > 0 || progressAmountToRemove > 0)
		{
			InterpProgressMeter();
		}

		// Handle jump cooldown, it is .5s
		if (jumpCooldown > 0 && !gamePaused && !isSupportPaused)
		{
			jumpCooldown -= 1;
			UIWidget->UpdateJumpCooldownBar((float)jumpCooldown / (float)30);
		}

		if (isInFever && !isInTranscendance && !gamePaused && !isSupportPaused)
		{
			if (feverTimeRemaining > 0)
			{
				feverTimeRemaining -= 1;
				UpdateFeverMeter();
			}
			else
			{
				ExitFever();
			}
		}
	}
}

void ATrampolineActor::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Select1", IE_Released, this, &ATrampolineActor::SelectOne);
	PlayerInputComponent->BindAction("Select2", IE_Released, this, &ATrampolineActor::SelectTwo);
	PlayerInputComponent->BindAction("Select3", IE_Released, this, &ATrampolineActor::SelectThree);
	PlayerInputComponent->BindAction("Select4", IE_Released, this, &ATrampolineActor::SelectFour);
	PlayerInputComponent->BindAction("Select5", IE_Released, this, &ATrampolineActor::SelectFive);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATrampolineActor::Trampoline);

	PlayerInputComponent->BindAction("Start", IE_Released, this, &ATrampolineActor::Start);
	PlayerInputComponent->BindAction("Start", IE_Released, this, &ATrampolineActor::CommitMeter);

	PlayerInputComponent->BindAction("SwitchDisplay", IE_Pressed, this, &ATrampolineActor::ToggleQuestionDisplay);

	PlayerInputComponent->BindAction("Fever", IE_Released, this, &ATrampolineActor::EnterFever);

	PlayerInputComponent->BindAction("Pause", IE_Released, this, &ATrampolineActor::GamePause);

	PlayerInputComponent->BindAction("NextDef", IE_Released, this, &ATrampolineActor::GetNextDefinition);
	PlayerInputComponent->BindAction("PrevDef", IE_Released, this, &ATrampolineActor::GetPreviousDefinition);
}

#pragma endregion setup/start

#pragma region flow

void ATrampolineActor::Start()
{
	if (!hasStarted && !isSettingLaunchMeter)
	{
		StartLaunchMeter();
	}
}

void ATrampolineActor::HandleLaunch()
{
	if (!launchMeterFirstPressCommited)
	{
		if (launchMeterFirstPressPercent < 1 && launchMeterFillingUp)
		{
			launchMeterFirstPressPercent += .05;
		}
		else if (launchMeterFirstPressPercent > 1 && launchMeterFillingUp)
		{
			launchMeterFirstPressPercent = 1;
			launchMeterFillingUp = false;
		}
		else if (launchMeterFirstPressPercent > 0 && !launchMeterFillingUp)
		{
			launchMeterFirstPressPercent -= .05;
		}
		else if (launchMeterFirstPressPercent < 0 && !launchMeterFillingUp)
		{
			launchMeterFirstPressPercent = 0;
			launchMeterFillingUp = true;
		}
	}
	else if (launchMeterFirstPressCommited && !launchMeterSecondPressCommited)
	{
		if (launchMeterSecondPressPercent < launchMeterFirstPressPercent && launchMeterFillingUp)
		{
			launchMeterSecondPressPercent += .05;
		}
		else if (launchMeterSecondPressPercent >= launchMeterFirstPressPercent && launchMeterFillingUp)
		{
			launchMeterSecondPressPercent = launchMeterFirstPressPercent;
			launchMeterFillingUp = false;
		}
		else if (launchMeterSecondPressPercent > 0 && !launchMeterFillingUp)
		{
			launchMeterSecondPressPercent -= .05;
		}
		else if (launchMeterSecondPressPercent < 0 && !launchMeterFillingUp)
		{
			launchMeterSecondPressPercent = 0;
			launchMeterFillingUp = true;
		}
	}
	else
	{
		DoLaunch();
		qActor->SetBoxEnabled(true);
	}
	UIWidget->UpdateLaunchUI(launchMeterFirstPressPercent, launchMeterFirstPressCommited, launchMeterSecondPressPercent, launchMeterSecondPressCommited);
	//qActor->SetBoxSize(launchMeterFirstPressPercent);
}

void ATrampolineActor::StartLaunchMeter()
{
	isSettingLaunchMeter = true;
	launchMeterVisible = true;
	doingInitCameraZoomout = true;
	qActor->PlayBaseParticleSystem();
	qActor->SetBoxEnabled(false);
	lunaPetAnimInstance->isCharging = true;
}

void ATrampolineActor::CommitMeter()
{
	if (!hasStarted && !launchDone)
	{
		if (!initPress)
		{
			initPress = true;
		}
		else if (!launchMeterFirstPressCommited)
		{
			qActor->PlayMidParticleSystem();
			launchMeterFirstPressCommited = true;
			launchMeterSecondPressPercent = launchMeterFirstPressPercent;
			launchMeterFillingUp = true;
		}
		else if (launchMeterFirstPressCommited && !launchMeterSecondPressCommited)
		{
			launchMeterSecondPressCommited = true;
		}
	}
}

void ATrampolineActor::DoLaunch()
{
	UIWidget->HideLaunchUI();
	qActor->PlayTopParticleSystem();
	qActor->StopBaseParticleSystem();
	PlayJumpParticleSystem(jumpParticleSystem);
	lunaPetAnimInstance->isJumping = true;
	isSettingLaunchMeter = false;
	GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, 1.f) * currentImpulseForce * 2 * (1 - (.2 * launchMeterSecondPressPercent)));
	UIWidget->UpdateLaunchUI(launchMeterFirstPressPercent, launchMeterFirstPressCommited, launchMeterSecondPressPercent, launchMeterSecondPressCommited);
	launchDone = true;
	hasStarted = true;
	AssignNewQuestion();
	UIWidget->ShowGameUI();
	GetCharacterMovement()->GravityScale = currentGravityMultiplier;
	UIWidget->EnableAllGameplayButtons();
}

void ATrampolineActor::GameOver()
{
	if (!gameOver)
	{
		UpdateLevelDetails();
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0);
		UIWidget->ShowGameOver(questionManager.GetNumberOfQuestionsAnsweredCorrectly() >= questionManager.minQuestionsToWin, questionManager.GetNumberOfQuestionsAnsweredCorrectly(), questionManager.minQuestionsToWin, score, gameInstance->scoreThresholdForClear, gameInstance->scoreThresholdFor2Star, gameInstance->scoreThresholdFor3Star);
		gameOver = true;
	}
}

void ATrampolineActor::GameWon()
{
	int bonusRewardRand = rand() % 10;
	if (bonusRewardRand > 8)
	{
		if (score > gameInstance->scoreThresholdFor3Star)
		{
			autoFeverHeld += 3;
			questionReduceHeld += 3;
			supportHeld += 3;
			UIWidget->ShowRewardUI(3);
		}
		else if (score > gameInstance->scoreThresholdFor2Star)
		{
			autoFeverHeld += 2;
			questionReduceHeld += 2;
			supportHeld += 2;
			UIWidget->ShowRewardUI(2);
		}
		else if (score > gameInstance->scoreThresholdForClear)
		{
			autoFeverHeld += 1;
			questionReduceHeld += 1;
			supportHeld += 1;
			UIWidget->ShowRewardUI(1);
		}
	}

	gameWon = true;
	UpdateLevelDetails();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0);
	UIWidget->ShowGameWon(questionManager.GetNumberOfQuestionsAnsweredCorrectly() >= questionManager.minQuestionsToWin, questionManager.GetNumberOfQuestionsAnsweredCorrectly(), questionManager.minQuestionsToWin, score, gameInstance->scoreThresholdForClear, gameInstance->scoreThresholdFor2Star, gameInstance->scoreThresholdFor3Star);
}

void ATrampolineActor::DoSupportPause()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0);
	isSupportPaused = true;
	UIWidget->DoSupportAnimation(supportIndex);
	UIWidget->DisableAllGameplayButtons();
}

void ATrampolineActor::DoSupportResume()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	isSupportPaused = false;
	UIWidget->EnableAllGameplayButtons();
	
	switch (supportIndex)
	{
	case 0:
		DoSupport1();
		break;
	case 1:
		DoSupport2();
		break;
	case 2:
		DoSupport3();
		break;
	case 3:
		DoSupport4();
		break;
	case 4:
		DoSupport5();
		break;
	case 5:
		DoAbilityReduce();
		break;
	case 6:
		DoAbilityFever();
		break;
	case 10:
		DoEnterFever();
		break;
	case 11:
		DoEnterTranscendance();
		break;
	default:
		break;
	}
}

void ATrampolineActor::GamePause()
{
	if (!gameOver && !gameWon)
	{
		UpdateLevelDetails();
		if (gamePaused)
		{
			UIWidget->EnableAllGameplayButtons();
			gamePaused = false;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
			UIWidget->ShowGamePaused(false, questionManager.GetNumberOfQuestionsAnsweredCorrectly() >= questionManager.minQuestionsToWin, questionManager.GetNumberOfQuestionsAnsweredCorrectly(), questionManager.minQuestionsToWin, score, gameInstance->scoreThresholdForClear, gameInstance->scoreThresholdFor2Star, gameInstance->scoreThresholdFor3Star);
		}
		else
		{
			UIWidget->DisableAllGameplayButtons();
			gamePaused = true;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0);
			UIWidget->ShowGamePaused(true, questionManager.GetNumberOfQuestionsAnsweredCorrectly() >= questionManager.minQuestionsToWin, questionManager.GetNumberOfQuestionsAnsweredCorrectly(), questionManager.minQuestionsToWin, score, gameInstance->scoreThresholdForClear, gameInstance->scoreThresholdFor2Star, gameInstance->scoreThresholdFor3Star);
		}
	}
}

void ATrampolineActor::GamePauseFromUI()
{
	GamePause();
}

void ATrampolineActor::DetailedHistoryDisplay()
{
	UIWidget->ShowDetailedHistoryUI(true);
	isShowingDetailedHistory = true;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0);
}

void ATrampolineActor::DetailedHistoryHide()
{
	UIWidget->ShowDetailedHistoryUI(false);
	isShowingDetailedHistory = false;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
}

void ATrampolineActor::UpdateLevelDetails()
{
	gameInstance->previousLevelScore = score;
	gameInstance->previousLevelQuestionsCorrect = questionManager.GetNumberOfQuestionsAnsweredCorrectly();
	gameInstance->previousLevelAutoFeverRemain = autoFeverHeld;
	gameInstance->previousLevelQuestionReduceRemain = questionReduceHeld;
	gameInstance->previousLevelSupportRemain = supportHeld;

	if (questionManager.GetNumberOfQuestionsAnsweredCorrectly() >= questionManager.minQuestionsToWin)
	{
		gameInstance->wasLevelCompleted = true;
		if (score >= gameInstance->scoreThresholdForClear)
		{
			gameInstance->previousLevelStarsEarned = 1;
		}
		if (score >= gameInstance->scoreThresholdFor2Star)
		{
			gameInstance->previousLevelStarsEarned = 2;
		}
		if (score >= gameInstance->scoreThresholdFor3Star)
		{
			gameInstance->previousLevelStarsEarned = 3;
		}
	}
	else
	{
		gameInstance->wasLevelCompleted = false;
	}
}

void ATrampolineActor::Save()
{

}

#pragma endregion flow

// Do jump action
#pragma region Trampoline

void ATrampolineActor::JumpFromUI()
{
	if (hasStarted)
	{
		Trampoline();
	}
	else if (!hasStarted && !isSettingLaunchMeter)
	{
		Start();
	}
	else if (!hasStarted && !launchDone)
	{
		initPress = true;
		CommitMeter();
	}
}

void ATrampolineActor::Trampoline()
{

	// Check if in valid jump state
	if (jumpState == 0 && jumpCooldown == 0)
	{
		if (isInTranscendance)
		{
			ExitTranscendance();
		}
		if (isInFever)
		{
			ExitFever();
		}
		if (currentComboMultiplier == 1 || nonMultiplierQuestionsCorrect != 0)
		{
			UpdateProgressMeter(true);
			subsequentIncorrectQuestions += 1;
		}
		PlayJumpParticleSystem(jumpParticleSystem);
		lunaPetAnimInstance->isJumping = true;
		// Reset In-Between-Jump Multiplier
		currentComboMultiplier = 1.f;
		nonMultiplierQuestionsCorrect = 0;
		UIWidget->UpdateComboUI((int)currentComboMultiplier, (int)nonMultiplierQuestionsCorrect);
		isBuildingMultiplier = true;

		// Get rid of characters negative Z velocity
		GetCharacterMovement()->Velocity.Set(0, 0, 0);
		GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, 1.f) * currentImpulseForce);

		// Set jump state back to none and restart the jump cooldown to .5 seconds
		jumpState = -1;
		jumpCooldown = 30;
	}
	else if (jumpState == -2)
	{
		if (isInTranscendance)
		{
			ExitTranscendance();
		}
		if (isInFever)
		{
			ExitFever();
		}
		PlayJumpParticleSystem(groundParticleSystem);
		currentComboMultiplier = 1.f;
		nonMultiplierQuestionsCorrect = 0;
		subsequentIncorrectQuestions += 2;
		GetCharacterMovement()->Velocity.Set(0, 0, 0);
		GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, 1.f) * initialJumpPower);
		UIWidget->UpdateComboUI((int)currentComboMultiplier, (int)nonMultiplierQuestionsCorrect);
		UpdateProgressMeter(true);
		jumpState = 0;
		jumpCooldown = 30;
	}
	else // Jump outside of valid zone
	{
		// Invalid jump could have been an accidental press,
		// if invalid jump is inside the cooldown window, no penalty
		if (jumpCooldown == 0)
		{
			jumpCooldown = 30;
		}
	}
}

void ATrampolineActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherComp->GetName() == "Jump zone")
	{
		jumpState = 0;
	}
	else if (OtherComp->GetName() == "Ground zone")
	{
		//GEngine->AddOnScreenDebugMessage(8, 20, FColor::White, FString("Overlapped ground zone")); FScreenMessageString* mLate = GEngine->ScreenMessages.Find(8); mLate->TextScale.X = mLate->TextScale.Y = 1.0f;
		jumpState = -2;
		Trampoline();
	}
}

void ATrampolineActor::OnOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->GetName() == "Jump zone")
	{
		jumpState != -1 ? jumpState = -1 : jumpState;
	}
}

void ATrampolineActor::PlayJumpParticleSystem(UParticleSystem* pSystem)
{
	jumpParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pSystem, actorOverlapComponent->GetComponentLocation());
}

#pragma endregion Trampoline

// Manage character related properties
#pragma region Character

// Unused
void ATrampolineActor::UpdateCharacter(float multiplier)
{
	UpdateHealth(.05 * multiplier);
}

// Unused
void ATrampolineActor::UpdateHealth(float amount)
{
}

// Update progress meter to next level, boolean indicates whether progress should be negative
void ATrampolineActor::UpdateProgressMeter(bool negative)
{
	if (!negative)
	{
		float multiplierAffectedValue = currentComboMultiplier * singleQuestionReward;
		float updateValue = multiplierAffectedValue + (nonMultiplierQuestionsCorrect * .10);

		if (updateValue > 1)
		{
			updateValue = 1;
		}
		progressAmountToAdd += updateValue;
	}
	else
	{
		if (subsequentIncorrectQuestions > 0)
		{
			progressAmountToRemove += singleQuestionReward * (subsequentIncorrectQuestions);
		}
		else
		{
			progressAmountToRemove += singleQuestionReward;
		}
		if (isInFever)
		{
			ExitFever();
		}
		else if (isInTranscendance)
		{
			ExitTranscendance();
		}
	}
}

void ATrampolineActor::InterpProgressMeter()
{
	if (progressAmountToAdd > 0 && progressAmountToRemove == 0)
	{
		if (progressAmountToAdd > progressMeterIncreaseAmount)
		{
			progressMeter += progressMeterIncreaseAmount;
			progressAmountToAdd -= progressMeterIncreaseAmount;
			if (progressMeter > 1)
			{
				UpdateJumpLevel(1);
				progressMeter = (1 - progressMeter);
			}
		}
		else
		{
			progressMeter += progressAmountToAdd;
			progressAmountToAdd = 0;
			if (progressMeter > 1)
			{
				UpdateJumpLevel(1);
				progressMeter = (1 - progressMeter);
			}
		}
	}
	else if (progressAmountToRemove > 0)
	{
		if (progressAmountToRemove > progressMeterIncreaseAmount)
		{
			progressMeter -= progressMeterIncreaseAmount;
			progressAmountToRemove -= progressMeterIncreaseAmount;
			if (progressMeter < 0)
			{
				UpdateJumpLevel(-1);
				progressMeter = 1 - progressMeter;
			}
		}
		else
		{
			progressMeter -= progressAmountToRemove;
			progressAmountToRemove = 0;
			if (progressMeter < 0)
			{
				UpdateJumpLevel(-1);
				progressMeter = 1 - progressMeter;
			}
		}
	}

	UIWidget->UpdatePercentageUI(progressMeter);
}

// Update combo multuplier after answering questions
void ATrampolineActor::UpdateComboMultiplier(int increment)
{
	if (increment > 0)
	{
		currentComboMultiplier += increment;
	}

	UIWidget->UpdateComboUI((int)currentComboMultiplier, (int)nonMultiplierQuestionsCorrect);
}

// Update jump level, jump level with increase or decrease by the number passed in, depending on the sign
void ATrampolineActor::UpdateJumpLevel(float newJumpLevel)
{
	currentJumpLevel += newJumpLevel;

	if (currentJumpLevel < 1)
	{
		GameOver();
	}
	else
	{
		if (currentJumpLevel <= maxJumpLevel)
		{
			currentImpulseForce = initialJumpPower;
			for (int i = 1; i < currentJumpLevel; i++)
			{
				currentImpulseForce += (initialJumpPower * jumpPowerPerLevelMultiplier);
			}
			UIWidget->UpdateJumpLevelUI((int)currentJumpLevel);
		}
	}
}

void ATrampolineActor::UpdateScore()
{
	score += (int)currentComboMultiplier * singleQuestionScoreReward;
	UIWidget->UpdateScoreUI(score);
}

// Enter fever state
void ATrampolineActor::EnterFever()
{
	if (feverReady)
	{
		supportIndex = 10;
		DoSupportPause();
	}
}

void ATrampolineActor::DoEnterFever()
{
	feverExtendRewardCounter = 0;
	isInFever = true;
	isBuildingFever = false;
	feverTimeRemaining = maxFeverTimeRemaining;
	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .1);
	this->CustomTimeDilation = .2;
	PlayFeverActivatedParticleSystem(feverActivatedParticleSystem);
}

void ATrampolineActor::EnterFeverFromUI()
{
	EnterFever();
}

// Exit fever state
void ATrampolineActor::ExitFever()
{
	lunaPetAnimInstance->isFeverReady = false;
	isInFever = false;
	feverReady = false;
	feverMeter = 0.0;
	feverTimeRemaining = 0;
	feverExtendRewardCounter = 0;
	feverSubsequentCorrectQuestions = 0;
	transcendanceMeter = 0;
	UIWidget->UpdateTranscendanceUI(transcendanceMeter, isInTranscendance, feverSubsequentCorrectQuestions);
	UIWidget->UpdateFeverUI(feverMeter, feverReady, isInFever, feverExtendRewardCounter);
	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	PlayAnswerParticleSystem(incorrectParticleSystem);
	this->CustomTimeDilation = 1.f;
}

// Update fever meter for building or extending fever
void ATrampolineActor::UpdateFeverMeter()
{

	if (isBuildingFever)
	{
		if (subsequentCorrectQuestions < 5)
		{
			feverMeter += feverBaseReward;
		}
		else
		{
			feverMeter += feverBaseReward * (subsequentCorrectQuestions / feverQuestionsCorrectForReward);
		}
		if (feverMeter > 1)
		{
			feverMeter = 1;
			feverReady = true;
			lunaPetAnimInstance->isFeverReady = true;
		}
		UIWidget->UpdateFeverUI(feverMeter, feverReady, isInFever, feverExtendRewardCounter);
	}
	else
	{
		feverMeter = feverTimeRemaining / maxFeverTimeRemaining;
		UIWidget->UpdateFeverUI(feverMeter, feverReady, isInFever, feverExtendRewardCounter);
	}
}

void ATrampolineActor::PlayFeverActivatedParticleSystem(UParticleSystem* pSystem)
{
	feverActivatedParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(pSystem, actorOverlapComponent);
	feverActivatedParticleSystemComponent->SetRelativeRotation(FRotator::ZeroRotator);
}

void ATrampolineActor::SetActivePowerup(int powerup)
{
	currentPowerup = powerup;
}

void ATrampolineActor::DoPowerup()
{
	if (currentPowerup == 0 && autoFeverHeld > 0)
	{
		autoFeverHeld -= 1;
		supportIndex = 6;
		DoSupportPause();
	}
	else if (currentPowerup == 1 && questionReduceHeld > 0)
	{
		questionReduceHeld -= 1;
		supportIndex = 5;
		DoSupportPause();
	}
	else if (currentPowerup == 2 && supportHeld > 0)
	{
		int supportIndexRand = rand() % 10;

		if (supportIndexRand < 3)
		{
			supportIndex = 0;
		}
		else if (supportIndexRand < 5)
		{
			supportIndex = 1;
		}
		else if (supportIndexRand < 7)
		{
			supportIndex = 2;
		}
		else if (supportIndexRand < 9)
		{
			supportIndex = 3;
		}
		else
		{
			supportIndex = 4;
		}

		DoSupportPause();
		supportHeld -= 1;
	}
	UIWidget->UpdatePowerupUI(autoFeverHeld, questionReduceHeld, supportHeld);
}

void ATrampolineActor::DoSupport1()
{
	GetCharacterMovement()->Velocity.Set(0, 0, 0);
	GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, 1.f) * currentImpulseForce * .75);
	PlayJumpParticleSystem(jumpParticleSystem);
}

void ATrampolineActor::DoSupport2()
{
	currentComboMultiplier += 5;
	UIWidget->UpdateComboUI(currentComboMultiplier, nonMultiplierQuestionsCorrect);
	PlayAnswerParticleSystem(correctParticleSystem);
}

void ATrampolineActor::DoSupport3()
{
	progressAmountToAdd += 1;
	PlayAnswerParticleSystem(correctParticleSystem);
}

void ATrampolineActor::DoSupport4()
{
	questionReduceCharges = 3;
	DoQuestionReduceHighlight(true);
	PlayAnswerParticleSystem(correctParticleSystem);
}

void ATrampolineActor::DoSupport5()
{
	if (!isInFever)
	{
		feverMeter += .75;
		if (feverMeter > 1)
		{
			feverMeter = 1;
			feverReady = true;
			lunaPetAnimInstance->isFeverReady = true;
		}
		UIWidget->UpdateFeverUI(feverMeter, feverReady, isInFever, feverExtendRewardCounter);
	}
	else
	{
		feverTimeRemaining = maxFeverTimeRemaining;
	}
}

void ATrampolineActor::DoAbilityFever()
{
	if (!isInFever)
	{
		feverMeter = 1;
		feverReady = true;
		lunaPetAnimInstance->isFeverReady = true;
		UIWidget->UpdateFeverUI(feverMeter, feverReady, isInFever, feverExtendRewardCounter);
	}
	else
	{
		feverTimeRemaining = maxFeverTimeRemaining;
	}
}

void ATrampolineActor::DoAbilityReduce()
{
	int correctIndex = questionManager.GetCorrectIndex();
	int incorrectIndex1 = -1;
	int incorrectIndex2 = -1;
	while (incorrectIndex1 == -1 || incorrectIndex1 == correctIndex)
	{
		incorrectIndex1 = rand() % 4;
	}
	while (incorrectIndex2 == -1 || incorrectIndex2 == correctIndex || incorrectIndex2 == incorrectIndex1)
	{
		incorrectIndex2 = rand() % 4;
	}
	UIWidget->HighlightAnswers(questionManager.GetCorrectIndex(), incorrectIndex1, incorrectIndex2);
	questionReduceHighlightActive = true;
	questionReduceCharges = 4;
	PlayAnswerParticleSystem(correctParticleSystem);
}

// Enter transcendance state
void ATrampolineActor::EnterTranscendance()
{
	supportIndex = 11;
	DoSupportPause();
}

void ATrampolineActor::DoEnterTranscendance()
{
	isInTranscendance = true;
	feverMeter = 1;
	UIWidget->UpdateTranscendanceUI(transcendanceMeter, isInTranscendance, feverSubsequentCorrectQuestions);
	this->CustomTimeDilation = 1;
	GetCharacterMovement()->Velocity.Set(0, 0, 0);
	GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, 1.f) * currentImpulseForce);
	PlayJumpParticleSystem(jumpParticleSystem);
	PlayFeverActivatedParticleSystem(transcendanceActivatedParticleSystem);
	lunaPetAnimInstance->isTransendanceActive = true;
}

// Exit transcendance state
void ATrampolineActor::ExitTranscendance()
{
	isInTranscendance = false;
	isBuildingTranscendance = false;
	ExitFever();
	lunaPetAnimInstance->isTransendanceActive = false;
}

// Update transcendance meter
void ATrampolineActor::UpdateTranscendanceMeter()
{
	if (isBuildingTranscendance)
	{
		transcendanceMeter += transcendanceBuildReward * (feverSubsequentCorrectQuestions/10);
		if (transcendanceMeter >= 1)
		{
			EnterTranscendance();
		}
		UIWidget->UpdateTranscendanceUI(transcendanceMeter, isInTranscendance, feverSubsequentCorrectQuestions);
	}
}

#pragma endregion Character

// Handle question action
#pragma region Questions

// Assign a new question from the available list
void ATrampolineActor::AssignNewQuestion()
{
	questionManager.GetNewQuestion2();
	switch (questionManager.GetQuestionMode())
	{
	case 0: // Kanji/Hiragana question English answer
		UIWidget->UpdateCurrentQuestion(questionManager.GetCurrentQuestion().kanji);
		UIWidget->UpdateCurrentAnswers(questionManager.answerVector.at(0).definitionList[0], questionManager.answerVector.at(1).definitionList[0], questionManager.answerVector.at(2).definitionList[0], questionManager.answerVector.at(3).definitionList[0], questionManager.answerVector.at(4).definitionList[0]);
		break;
	case 1: // Kanji question with Hiragana answer
		UIWidget->UpdateCurrentQuestion(questionManager.GetCurrentQuestion().kanji);
		UIWidget->UpdateCurrentAnswers(questionManager.answerVector.at(0).hiragana, questionManager.answerVector.at(1).hiragana, questionManager.answerVector.at(2).hiragana, questionManager.answerVector.at(3).hiragana, questionManager.answerVector.at(4).hiragana);
		break;
	case 2: // Kanji/English question with hiragana answer
		UIWidget->UpdateCurrentQuestion(questionManager.GetCurrentQuestion().kanji);
		UIWidget->UpdateCurrentAnswers(questionManager.answerVector.at(0).hiragana, questionManager.answerVector.at(1).hiragana, questionManager.answerVector.at(2).hiragana, questionManager.answerVector.at(3).hiragana, questionManager.answerVector.at(4).hiragana);
		break;
	case 3: // English question with Kanji answer
		UIWidget->UpdateCurrentQuestion(questionManager.GetCurrentQuestion().english);
		UIWidget->UpdateCurrentAnswers(questionManager.answerVector.at(0).kanji, questionManager.answerVector.at(1).kanji, questionManager.answerVector.at(2).kanji, questionManager.answerVector.at(3).kanji, questionManager.answerVector.at(4).kanji);
		break;
	default:
		break;
	}
	if (questionReduceCharges > 0)
	{
		UIWidget->ResetAnswerColors();
		DoQuestionReduceHighlight(false);
	}
}

void ATrampolineActor::GetNextDefinition()
{
	switch (questionManager.GetQuestionMode())
	{
	case 0: // Kanji/Hiragana question English answer
		UIWidget->UpdateCurrentAnswers(questionManager.GetNextDef(questionManager.answerVector.at(0)), questionManager.GetNextDef(questionManager.answerVector.at(1)), questionManager.GetNextDef(questionManager.answerVector.at(2)), questionManager.GetNextDef(questionManager.answerVector.at(3)), questionManager.GetNextDef(questionManager.answerVector.at(4)));
		break;
	default:
		break;
	}

	if (historyVector.size() < 5)
	{
		switch (historyVector.size())
		{
		case 1:
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 2:
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 3:
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 4:
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, historyVector.at(3).kanji, historyVector.at(3).definitionList[historyVector.at(3).currentDefinitionListIndex], historyVector.at(3).justAnsweredCorrectly, FString(""), FString(""), false);
			break;
		default:
			break;
		}
	}
	else
	{
		UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, historyVector.at(3).kanji, historyVector.at(3).definitionList[historyVector.at(3).currentDefinitionListIndex], historyVector.at(3).justAnsweredCorrectly, historyVector.at(4).kanji, historyVector.at(4).definitionList[historyVector.at(4).currentDefinitionListIndex], historyVector.at(4).justAnsweredCorrectly);
	}

}

void ATrampolineActor::GetPreviousDefinition()
{
	switch (questionManager.GetQuestionMode())
	{
	case 0: // Kanji/Hiragana question English answer
		UIWidget->UpdateCurrentAnswers(questionManager.GetPrevDef(questionManager.answerVector.at(0)), questionManager.GetPrevDef(questionManager.answerVector.at(1)), questionManager.GetPrevDef(questionManager.answerVector.at(2)), questionManager.GetPrevDef(questionManager.answerVector.at(3)), questionManager.GetPrevDef(questionManager.answerVector.at(4)));
		break;
	default:
		break;
	}

	if (historyVector.size() < 5)
	{
		switch (historyVector.size())
		{
		case 1:
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 2:
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 3:
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 4:
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, historyVector.at(3).kanji, historyVector.at(3).definitionList[historyVector.at(3).currentDefinitionListIndex], historyVector.at(3).justAnsweredCorrectly, FString(""), FString(""), false);
			break;
		default:
			break;
		}
	}
	else
	{
		UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, historyVector.at(3).kanji, historyVector.at(3).definitionList[historyVector.at(3).currentDefinitionListIndex], historyVector.at(3).justAnsweredCorrectly, historyVector.at(4).kanji, historyVector.at(4).definitionList[historyVector.at(4).currentDefinitionListIndex], historyVector.at(4).justAnsweredCorrectly);
	}
}

// Update the last 5 question history
void ATrampolineActor::UpdateHistory(Question &questionToAdd)
{
	if (historyVector.size() < 5)
	{
		switch (historyVector.size())
		{
		case 0:
			historyVector.push_back(questionToAdd);
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 1:
			historyVector.push_back(historyVector.at(0));
			historyVector.at(0) = questionToAdd;
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 2:
			historyVector.push_back(historyVector.at(1));
			historyVector.at(1) = historyVector.at(0);
			historyVector.at(0) = questionToAdd;
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, FString(""), FString(""), false, FString(""), FString(""), false);
			break;
		case 3:
			historyVector.push_back(historyVector.at(2));
			historyVector.at(2) = historyVector.at(1);
			historyVector.at(1) = historyVector.at(0);
			historyVector.at(0) = questionToAdd;
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, historyVector.at(3).kanji, historyVector.at(3).definitionList[historyVector.at(3).currentDefinitionListIndex], historyVector.at(3).justAnsweredCorrectly, FString(""), FString(""), false);
			break;
		case 4:
			historyVector.push_back(historyVector.at(3));
			historyVector.at(3) = historyVector.at(2);
			historyVector.at(2) = historyVector.at(1);
			historyVector.at(1) = historyVector.at(0);
			historyVector.at(0) = questionToAdd;
			UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, historyVector.at(3).kanji, historyVector.at(3).definitionList[historyVector.at(3).currentDefinitionListIndex], historyVector.at(3).justAnsweredCorrectly, historyVector.at(4).kanji, historyVector.at(4).definitionList[historyVector.at(4).currentDefinitionListIndex], historyVector.at(4).justAnsweredCorrectly);
			break;
		default:
			break;
		}
	}
	else
	{
		historyVector.pop_back();
		historyVector.push_back(historyVector.at(3));
		historyVector.at(3) = historyVector.at(2);
		historyVector.at(2) = historyVector.at(1);
		historyVector.at(1) = historyVector.at(0);
		historyVector.at(0) = questionToAdd;
		UIWidget->UpdateQuestionHistory(historyVector.at(0).kanji, historyVector.at(0).definitionList[historyVector.at(0).currentDefinitionListIndex], historyVector.at(0).justAnsweredCorrectly, historyVector.at(1).kanji, historyVector.at(1).definitionList[historyVector.at(1).currentDefinitionListIndex], historyVector.at(1).justAnsweredCorrectly, historyVector.at(2).kanji, historyVector.at(2).definitionList[historyVector.at(2).currentDefinitionListIndex], historyVector.at(2).justAnsweredCorrectly, historyVector.at(3).kanji, historyVector.at(3).definitionList[historyVector.at(3).currentDefinitionListIndex], historyVector.at(3).justAnsweredCorrectly, historyVector.at(4).kanji, historyVector.at(4).definitionList[historyVector.at(4).currentDefinitionListIndex], historyVector.at(4).justAnsweredCorrectly);
	}
}

FString ATrampolineActor::GetHistoryKanji(int index)
{
	return historyVector.at(index).kanji;
}

FString ATrampolineActor::GetHistoryHiragana(int index)
{
	return historyVector.at(index).hiragana;
}

FString ATrampolineActor::GetHistoryEnglish(int index)
{
	return historyVector.at(index).english;
}

// Get the first definition from a comma delimeted string
FString ATrampolineActor::GetFirstDef(const FString &def)
{
	FString firstDef = "";
	for (int i = 0; i < def.Len(); i++)
	{
		if (def[i] == *UTF8_TO_TCHAR(",") && i != 0)
		{
			break;
		}
		else if (def[i] != *UTF8_TO_TCHAR(","))
		{
			firstDef.AppendChar(def[i]);
		}
	}
	return firstDef;
}

// Do selection action
#pragma region Selections

// Select option 1
void ATrampolineActor::SelectOne()
{
	if (canMakeSelection)
		HandleSelection(0);
}

// Select option 2
void ATrampolineActor::SelectTwo()
{
	if (canMakeSelection)
		HandleSelection(1);
}

// Select option 3
void ATrampolineActor::SelectThree()
{
	if (canMakeSelection)
		HandleSelection(2);
}

// Select option 4
void ATrampolineActor::SelectFour()
{
	if (canMakeSelection)
		HandleSelection(3);
}

// Select option 5
void ATrampolineActor::SelectFive()
{
	if (canMakeSelection)
		HandleSelection(4);
}

// Handle selection
void ATrampolineActor::HandleSelection(int index)
{
	if (answeredWrong && !acknowledgedCorrectAnswer)
	{
		if (index == questionManager.GetCorrectIndex())
		{
			ExitAcknowledgeMode();
		}
		else
		{
			UIWidget->DoAnswerPulse(false);
		}
	}
	else
	{
		canMakeSelection = false;

		if (questionReduceHighlightActive && questionReduceCharges == 0)
		{
			UIWidget->ResetAnswerColors();
			questionReduceHighlightActive = false;
		}
		if (index == questionManager.GetCorrectIndex())
		{
			// correct answer
			PlayAnswerParticleSystem(correctParticleSystem);
			UpdateProgressMeter(false);
			UIWidget->DoAnswerPulse(true);
			subsequentIncorrectQuestions = 0;
			questionManager.lastQuestionAnsweredCorrectly = true;
			UpdateScore();

			if (isInTranscendance)
			{
				GetCharacterMovement()->Velocity.Set(0, 0, 0);
				GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, 1.f) * currentImpulseForce * .5);
				PlayJumpParticleSystem(jumpParticleSystem);
				subsequentCorrectQuestions += 1;
			}
			else if (isInFever)
			{
				feverSubsequentCorrectQuestions += 1;
				feverExtendRewardCounter += 1;
				subsequentCorrectQuestions += 1;
				if (feverExtendRewardCounter >= feverQuestionsCorrectForReward)
				{
					feverExtendRewardCounter = 0;
					feverTimeRemaining += feverExtendReward;
					if (feverTimeRemaining > maxFeverTimeRemaining)
					{
						feverTimeRemaining = maxFeverTimeRemaining;
					}
				}
				if (feverSubsequentCorrectQuestions >= questionsCorrectToBuildTranscendance)
				{
					isBuildingTranscendance = true;
					UpdateTranscendanceMeter();
				}
				UpdateFeverMeter();
			}
			else
			{
				subsequentCorrectQuestions += 1;
				feverExtendRewardCounter += 1;
				if (feverExtendRewardCounter >= feverQuestionsCorrectForReward)
				{
					feverExtendRewardCounter = 0;
					isBuildingFever = true;
					UpdateFeverMeter();
				}
			}

			if (isBuildingMultiplier)
			{
				UpdateComboMultiplier(1);
			}
			else
			{
				nonMultiplierQuestionsCorrect += 1;
				UpdateComboMultiplier(-1);
			}

			Question historyQuestion;
			historyQuestion = questionManager.GetCurrentQuestion();
			historyQuestion.justAnsweredCorrectly = true;
			UpdateHistory(historyQuestion);

			questionManager.IncrementQuestionsAnsweredCorrectly();
			if (questionManager.GetNumberOfQuestionsAnsweredCorrectly() >= questionManager.minQuestionsToWin)
			{
				UIWidget->ShowGamePassed();
				if (questionManager.GetNumberOfQuestionsAnsweredCorrectly() >= questionManager.totalQuestionsToRead)
				{
					GameWon();
				}
			}
		}
		else
		{
			// incorrect answer
			answeredWrong = true;
			acknowledgedCorrectAnswer = false;
			PlayAnswerParticleSystem(incorrectParticleSystem);
			UpdateProgressMeter(true);
			UIWidget->DoAnswerPulse(false);
			questionManager.lastQuestionAnsweredCorrectly = false;
			subsequentCorrectQuestions = 0;
			feverSubsequentCorrectQuestions = 0;
			feverExtendRewardCounter = 0;
			subsequentIncorrectQuestions += 1;
			isBuildingMultiplier = false;
			if (isInFever)
			{
				if (subsequentIncorrectQuestions >= subsequentIncorrectQuestionsToExitFever)
				{
					ExitFever();
				}
			}
			if (isInTranscendance)
			{
				ExitTranscendance();
			}
			Question historyQuestion;
			historyQuestion = questionManager.GetCurrentQuestion();
			historyQuestion.justAnsweredCorrectly = false;
			UpdateHistory(historyQuestion);

			EnterAcknowledgeMode();
		}

		UIWidget->UpdateTotalQuestionsUI(questionManager.GetNumberOfQuestionsAnsweredCorrectly(), questionManager.minQuestionsToWin, questionManager.totalQuestionsToRead);
		UIWidget->UpdateFeverUI(feverMeter, feverReady, isInFever, feverExtendRewardCounter);

		if (!gameWon && !answeredWrong)
		{
			AssignNewQuestion();
		}

		canMakeSelection = true;
	}
}

void ATrampolineActor::EnterAcknowledgeMode()
{
	UIWidget->DoAcknowledgeMode(questionManager.GetCorrectIndex());
	this->CustomTimeDilation = .2;
}

void ATrampolineActor::ExitAcknowledgeMode()
{
	UIWidget->ExitAcknowledgeMode(questionManager.GetCorrectIndex());
	this->CustomTimeDilation = 1;
	acknowledgedCorrectAnswer = true;
	answeredWrong = false;
	AssignNewQuestion();
}

void ATrampolineActor::DoQuestionReduceHighlight(bool support)
{
	if (questionReduceCharges > 0 && !support)
	{
		int correctIndex = questionManager.GetCorrectIndex();
		int incorrectIndex1 = -1;
		int incorrectIndex2 = -1;
		while (incorrectIndex1 == -1 || incorrectIndex1 == correctIndex)
		{
			incorrectIndex1 = rand() % 4;
		}
		while (incorrectIndex2 == -1 || incorrectIndex2 == correctIndex || incorrectIndex2 == incorrectIndex1)
		{
			incorrectIndex2 = rand() % 4;
		}
		UIWidget->HighlightAnswers(questionManager.GetCorrectIndex(), incorrectIndex1, incorrectIndex2);
		questionReduceHighlightActive = true;
		questionReduceCharges -= 1;
	}
	else if (questionReduceCharges > 0 && support)
	{
		int correctIndex = questionManager.GetCorrectIndex();
		int incorrectIndex1 = -1;
		int incorrectIndex2 = -1;
		while (incorrectIndex1 == -1 || incorrectIndex1 == correctIndex)
		{
			incorrectIndex1 = rand() % 4;
		}
		UIWidget->HighlightAnswers(questionManager.GetCorrectIndex(), incorrectIndex1, incorrectIndex1);
		questionReduceHighlightActive = true;
		questionReduceCharges -= 1;
	}
}

void ATrampolineActor::PlayAnswerParticleSystem(UParticleSystem* pSystem)
{
	answerParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(pSystem, actorOverlapComponent);
}

#pragma endregion Selections

#pragma endregion Questions

#pragma region Camera

void ATrampolineActor::SetCameraOrthoWidth(float dTime)
{
	if (GetCapsuleComponent()->GetPhysicsLinearVelocity().Z > 0)
	{
		SideViewCameraComponent->OrthoWidth = FMath::FInterpConstantTo(SideViewCameraComponent->OrthoWidth, maxCameraOrthoWidth, dTime, 100);
	}
	else if (GetCapsuleComponent()->GetPhysicsLinearVelocity().Z < 0)
	{
		SideViewCameraComponent->OrthoWidth = FMath::FInterpConstantTo(SideViewCameraComponent->OrthoWidth, defaultCameraOrthoWidth, dTime, 100);
	}
}

void ATrampolineActor::SetCameraBoomLength(float dTime, bool init)
{
	if (init)
	{
		if (CameraBoom->TargetArmLength< cameraMaxDistance)
		{
			CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, cameraMaxDistance, dTime, 1);
		}
		else
		{
			doingInitCameraZoomout = false;
			initCameraZoomoutComplete = true;
		}
	}

}

#pragma endregion Camera

#pragma region Blueprint

float ATrampolineActor::GetHealth()
{
	return health;
}

float ATrampolineActor::GetProgress()
{
	return progressMeter;
}

float ATrampolineActor::GetCurrentComboMult()
{
	return floorf(currentComboMultiplier);
}

float ATrampolineActor::GetCurrentJumpLevelMaxHeight()
{
	return currentJumpLevelMaxHeight;
}

float ATrampolineActor::GetCurrentHeight()
{
	return actorOverlapComponent->GetComponentLocation().Z;
}

float ATrampolineActor::GetAltitudeIndicatorOffset()
{
	return ((actorOverlapComponent->GetComponentLocation().Z / currentJumpLevelMaxHeight) * -1000);
}

float ATrampolineActor::GetCurrentJumpLevel()
{
	return currentJumpLevel;
}

void ATrampolineActor::SelectionMadeFromUI(int index)
{
	if (canMakeSelection)
	HandleSelection(index);
}

void ATrampolineActor::ToggleQuestionDisplay()
{
	switch (questionManager.GetQuestionMode())
	{
	case 0: // Kanji/Hiragana
		if (questionDisplayMode == 0)
		{
			UIWidget->UpdateCurrentQuestion(questionManager.GetCurrentQuestion().hiragana);
			questionDisplayMode = 1;
		}
		else
		{
			UIWidget->UpdateCurrentQuestion(questionManager.GetCurrentQuestion().kanji);
			questionDisplayMode = 0;
		}
		break;
	case 1: //Kanji only no  change
		break;
	case 2: // Kanji/English
		if (questionDisplayMode == 0)
		{
			UIWidget->UpdateCurrentQuestion(questionManager.GetCurrentQuestion().english);
			questionDisplayMode = 1;
		}
		else
		{
			UIWidget->UpdateCurrentQuestion(questionManager.GetCurrentQuestion().kanji);
			questionDisplayMode = 0;
		}
		break;
	default:
		break;
	}
}


int ATrampolineActor::GetLevelID()
{
	return gameInstance->levelID;
}
#pragma endregion Blueprint