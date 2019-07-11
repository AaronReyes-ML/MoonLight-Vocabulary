// Fill out your copyright notice in the Description page of Project Settings.

#include "OverworldPawn.h"
#include "Components/CapsuleComponent.h"
#include "Classes/Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SplineComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine.h"
#include "ConstructorHelpers.h"
#include "Junction.h"
#include "LevelPrompt.h"
#include "TrampolineOverworldWidget.h"
#include "TrampolineGameInstance.h"
#include "TrampolineSaveGame.h"
#include "WolfAnimInstance.h"
#include "StarGate.h"
#include "ItemPickup.h"
#include "Particles/ParticleSystemComponent.h"
#include "LevelTransitionActor.h"

// Sets default values
AOverworldPawn::AOverworldPawn()
{

	AutoPossessPlayer = EAutoReceiveInput::Player0;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	lunaPet = CreateDefaultSubobject<USkeletalMeshComponent>("Luna's pet");
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skelMeshObj0(TEXT("/Game/ParagonShinbi/Characters/Heroes/Shinbi/Shinbi_Wolf/Meshes/Shinbi_Wolf"));
	lunaPet->SetSkeletalMesh(skelMeshObj0.Object);
	lunaPet->SetRelativeLocation(FVector::ZeroVector);
	lunaPet->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetRootComponent(lunaPet);
	lunaPet->AddLocalOffset(FVector(0, 0, 100));

	static ConstructorHelpers::FClassFinder<UObject> animBPClassObj0(TEXT("/Game/ParagonShinbi/Characters/Heroes/Shinbi/Shinbi_Wolf/Animations/Wolf_OverwoldAnimInst_BP"));
	lunaPet->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	lunaPet->SetAnimInstanceClass(animBPClassObj0.Class);

	fogClearedParticleSystem = CreateDefaultSubobject<UParticleSystem>("Fog cleared particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> psO1(TEXT("/Game/Trampoline/Particles/MagicCircles/MCircle_FeverActivated"));
	fogClearedParticleSystem = psO1.Object;

	overlapComponent = CreateDefaultSubobject<UCapsuleComponent>("Overlap Component");
	overlapComponent->SetCapsuleRadius(65);
	overlapComponent->SetCapsuleHalfHeight(100);
	overlapComponent->SetupAttachment(RootComponent);
	overlapComponent->SetRelativeLocation(FVector::ZeroVector);
	overlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	overlapComponent->SetCollisionObjectType(ECC_GameTraceChannel2);
	overlapComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	overlapComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	overlapComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Overlap);
	overlapComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);
	overlapComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel5, ECR_Overlap);
	
	springArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	springArmComponent->SetupAttachment(RootComponent);
	springArmComponent->SetRelativeLocation(FVector::ZeroVector);
	springArmComponent->TargetArmLength = 1250;
	springArmComponent->SetWorldRotation(FRotator(-75.f, -90, 0));
	springArmComponent->bInheritPitch = false;
	springArmComponent->bInheritRoll = false;
	springArmComponent->bInheritYaw = false;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	cameraComponent->SetupAttachment(springArmComponent, USpringArmComponent::SocketName);

	lunaPetLight = CreateDefaultSubobject<UPointLightComponent>("Luna's pet's light");
	lunaPetLight->SetupAttachment(lunaPet);
	lunaPetLight->SetRelativeLocation(FVector::ZeroVector);
	lunaPetLight->AddLocalOffset(FVector(0, 0, 100));
	lunaPetLight->SetIntensity(2500);

}

// Called when the game starts or when spawned
void AOverworldPawn::BeginPlay()
{
	Super::BeginPlay();

	if (lunaPet)
	{
		lunaPetAnimInstance = Cast<UWolfAnimInstance>(lunaPet->GetAnimInstance());
	}

	UIWidget = Cast<UTrampolineOverworldWidget>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	if (UIWidget)
	{
		UIWidget->AddToViewport(-15);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Cyan, "failed to add ui widget");
	}

	gameInstance = Cast<UTrampolineGameInstance>(GetGameInstance());

	if (gameInstance)
	{
		localSaveSlotName = gameInstance->instancedSaveSlotName;
	}

	SetInitialCharacterLocation();
	RefreshAllLevelPrompts();
	RefreshAllItemPickups();

	if (gameInstance->cameFromLevel)
	{
		UpdateCompletedLevelDetails();
		RefreshAllLevelPrompts();
	}

	RefreshAllStarGates();

	TArray<AActor*> overlappingLevelPromptArray;
	overlapComponent->GetOverlappingActors(overlappingLevelPromptArray, ALevelPrompt::StaticClass());

	if (overlappingLevelPromptArray.Num() > 0)
	{
		isLevelPrompted = true;
		currentLevelPrompt = Cast<ALevelPrompt>(overlappingLevelPromptArray[0]);
		UIWidget->ShowLevelPrompt(currentLevelPrompt);
		overlapComponent->UpdateOverlaps();
	}

	UIWidget->UpdateTotalStarCount(totalStarCount);
	UIWidget->UpdatePowerupCount(totalAutoFeverHeld, totalQuestionReduceHeld, totalSupportHeld);
	UIWidget->UpdateBossKeyCount(totalBossKeys);

	overlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AOverworldPawn::OnOverlapBegin);
	overlapComponent->OnComponentEndOverlap.AddDynamic(this, &AOverworldPawn::OnOverlapEnd);	

	APlayerController* menuController = GetWorld()->GetFirstPlayerController();
	menuController->bShowMouseCursor = true;
	menuController->bEnableClickEvents = true;
	menuController->bEnableMouseOverEvents = true;
}

void AOverworldPawn::SetInitialCharacterLocation()
{
	// Load game
	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

	if (gameInstance->cameFromOverworldLevel)
	{
		if (loadGameInstance->levelID == (loadGameInstance->previousLevelID + 1))
		{
			currentJunction = startJunction;
			currentSpline = startJunction->GetUpSpline();

			distanceTraveledOnCurrentSpline = 300;
			lunaPet->SetWorldLocation(currentSpline->GetLocationAtDistanceAlongSpline(distanceTraveledOnCurrentSpline, ESplineCoordinateSpace::World));
		}
		else
		{
			currentJunction = endJunction;
			switch (forwardLevelTransitionActor->GetEndSplineIndex())
			{
			case 0:
				currentSpline = endJunction->GetUpSpline();
				break;
			case 1:
				currentSpline = endJunction->GetLeftSpline();
				break;
			case 2:
				currentSpline = endJunction->GetDownSpline();
				break;
			case 3:
				currentSpline = endJunction->GetRightSpline();
				break;
			default:
				break;
			}

			distanceTraveledOnCurrentSpline = currentSpline->GetSplineLength() - 300;
			lunaPet->SetWorldLocation(currentSpline->GetLocationAtDistanceAlongSpline(distanceTraveledOnCurrentSpline, ESplineCoordinateSpace::World));
		}
		gameInstance->cameFromOverworldLevel = false;
		gameInstance->cameFromLevel = false;
	}
	else
	{
		// Get saved junction from save file
		TArray<AActor*> objectArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AJunction::StaticClass(), objectArray);

		for (int i = 0; i < objectArray.Num(); i++)
		{
			if (loadGameInstance->savedJunctionID == Cast<AJunction>(objectArray[i])->GetJunctionID())
			{
				currentJunction = Cast<AJunction>(objectArray[i]);
			}
		}

		bool eUp = false;
		bool eLeft = false;
		bool eDown = false;
		bool eRight = false;

		currentJunction->GetEntranceDirection(eUp, eLeft, eDown, eRight);

		if (currentJunction)
		{
			switch (loadGameInstance->savedSplineID)
			{
			case 0:
				if (!eUp)
				{
					currentSpline = currentJunction->GetUpSpline();
				}
				else
				{
					currentSpline = currentJunction->GetUpJunction()->GetDownSpline();
				}
				break;
			case 1:
				if (!eLeft)
				{
					currentSpline = currentJunction->GetLeftSpline();
				}
				else
				{
					currentSpline = currentJunction->GetLeftJunction()->GetRightSpline();
				}
				break;
			case 2:
				if (!eDown)
				{
					currentSpline = currentJunction->GetDownSpline();
				}
				else
				{
					currentSpline = currentJunction->GetDownJunction()->GetUpSpline();
				}
				break;
			case 3:
				if (!eRight)
				{
					currentSpline = currentJunction->GetRightSpline();
				}
				else
				{
					currentSpline = currentJunction->GetRightJunction()->GetLeftSpline();
				}
				break;
			default:
				break;
			}
			if (loadGameInstance->savedCharacterDistanceOnSpline > 75 && loadGameInstance->savedCharacterDistanceOnSpline < currentSpline->GetSplineLength() - 75)
			{
				distanceTraveledOnCurrentSpline = loadGameInstance->savedCharacterDistanceOnSpline;
			}
			else if (loadGameInstance->savedCharacterDistanceOnSpline < 75)
			{
				distanceTraveledOnCurrentSpline = 75;
			}
			else
			{
				distanceTraveledOnCurrentSpline = currentSpline->GetSplineLength() - 75;
			}

			lunaPet->SetWorldLocation(currentSpline->GetLocationAtDistanceAlongSpline(distanceTraveledOnCurrentSpline, ESplineCoordinateSpace::World));

			TArray<AActor*> overlappingStarGateArray;
			overlapComponent->GetOverlappingActors(overlappingStarGateArray,AStarGate::StaticClass());

			if (overlappingStarGateArray.Num() > 0)
			{
				distanceTraveledOnCurrentSpline = distanceTraveledOnCurrentSpline - 260;
				if (distanceTraveledOnCurrentSpline < 75)
				{
					distanceTraveledOnCurrentSpline = 75;
				}
				lunaPet->SetWorldLocation(currentSpline->GetLocationAtDistanceAlongSpline(distanceTraveledOnCurrentSpline, ESplineCoordinateSpace::World));
			}
		}
	}

	levelID = loadGameInstance->levelID;
	totalStarCount = loadGameInstance->playerTotalStars;
	totalAutoFeverHeld = loadGameInstance->playerTotalAutoFeverHeld;
	totalQuestionReduceHeld = loadGameInstance->playerTotalQuestionReduceHeld;
	totalSupportHeld = loadGameInstance->playerTotalSupportHeld;
	totalBossKeys = loadGameInstance->levelKeysHeld;
	UIWidget->UpdateLevelText(loadGameInstance->levelID);
}

void AOverworldPawn::UpdateCompletedLevelDetails()
{
	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

	TArray<AActor*> objectArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelPrompt::StaticClass(), objectArray);

	ALevelPrompt* savedLevelPrompt = Cast<ALevelPrompt>(objectArray[0]);

	for (int i = 0; i < objectArray.Num(); i++)
	{
		if (loadGameInstance->savedLevelPromptID == Cast<ALevelPrompt>(objectArray[i])->GetLevelID())
		{
			savedLevelPrompt = Cast<ALevelPrompt>(objectArray[i]);
		}
	}



	totalAutoFeverHeld = gameInstance->previousLevelAutoFeverRemain;
	totalQuestionReduceHeld = gameInstance->previousLevelQuestionReduceRemain;
	totalSupportHeld = gameInstance->previousLevelSupportRemain;

	if (savedLevelPrompt)
	{
		if (savedLevelPrompt->GetIsBossLevel())
		{
			if (gameInstance->wasLevelCompleted)
			{
				if (loadGameInstance)
				{
					loadGameInstance->levelKeysHeld = savedLevelPrompt->GetBossKeyReward();
					totalBossKeys = savedLevelPrompt->GetBossKeyReward();
				}
			}
		}
		if (!savedLevelPrompt->hasBeenCompleted)
		{
			if (gameInstance->wasLevelCompleted)
			{
				savedLevelPrompt->hasBeenCompleted = true;
				savedLevelPrompt->starsEarned = gameInstance->previousLevelStarsEarned;
				totalStarCount += gameInstance->previousLevelStarsEarned;
				savedLevelPrompt->completedScore = gameInstance->previousLevelScore;
				savedLevelPrompt->completedQuestionsCorrect = gameInstance->previousLevelQuestionsCorrect;
				if (savedLevelPrompt->isPlayingParticles)
				{
					savedLevelPrompt->StopLevelPromptParticles();
				}
			}
		}
		else
		{
			if (gameInstance->wasLevelCompleted)
			{
				if (gameInstance->previousLevelScore > savedLevelPrompt->completedScore)
				{
					savedLevelPrompt->completedScore = gameInstance->previousLevelScore;
				}
				if (gameInstance->previousLevelQuestionsCorrect > savedLevelPrompt->completedQuestionsCorrect)
				{
					savedLevelPrompt->completedQuestionsCorrect = gameInstance->previousLevelQuestionsCorrect;
				}
				if (gameInstance->previousLevelStarsEarned > savedLevelPrompt->starsEarned)
				{
					totalStarCount += (gameInstance->previousLevelStarsEarned - savedLevelPrompt->starsEarned);
					savedLevelPrompt->starsEarned = gameInstance->previousLevelStarsEarned;
					UIWidget->UpdateTotalStarCount(totalStarCount);
				}
				if (savedLevelPrompt->isPlayingParticles)
				{
					savedLevelPrompt->StopLevelPromptParticles();
				}
			}
		}
	}

	if (loadGameInstance)
	{
		loadGameInstance->levelPromptArray[savedLevelPrompt->levelID] = FLevelCriticalInformationStruct(savedLevelPrompt->hasBeenCompleted, savedLevelPrompt->completedScore, savedLevelPrompt->completedQuestionsCorrect, savedLevelPrompt->starsEarned);
		loadGameInstance->playerTotalStars = totalStarCount;
		loadGameInstance->playerTotalAutoFeverHeld = totalAutoFeverHeld;
		loadGameInstance->playerTotalQuestionReduceHeld = totalQuestionReduceHeld;
		loadGameInstance->playerTotalSupportHeld = totalSupportHeld;
		UGameplayStatics::SaveGameToSlot(loadGameInstance, localSaveSlotName, loadGameInstance->UserIndex);
	}

}

void AOverworldPawn::RefreshAllLevelPrompts()
{
	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

	TArray<AActor*> objectArray5;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelPrompt::StaticClass(), objectArray5);

	if (loadGameInstance)
	{
		for (int i = 0; i < objectArray5.Num(); i++)
		{
			ALevelPrompt* temp = Cast<ALevelPrompt>(objectArray5[i]);
			temp->hasBeenCompleted = loadGameInstance->levelPromptArray[temp->levelID].completed;
			temp->completedScore = loadGameInstance->levelPromptArray[temp->levelID].score;
			temp->completedQuestionsCorrect = loadGameInstance->levelPromptArray[temp->levelID].questionsCorrect;
			temp->starsEarned = loadGameInstance->levelPromptArray[temp->levelID].starsEarned;

			if (!temp->hasBeenCompleted)
			{
				temp->DoPlayLevelPromptParticles();
			}
		}
	}
}

void AOverworldPawn::RefreshAllItemPickups()
{
	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

	TArray<AActor*> objectArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemPickup::StaticClass(), objectArray);

	if (loadGameInstance)
	{
		for (int i = 0; i < loadGameInstance->itemPickupArray.Num(); i++)
		{
			for (int j = 0; j < objectArray.Num(); j++)
			{
				AItemPickup* temp = Cast<AItemPickup>(objectArray[j]);
				if (temp)
				{
					if (temp->GetItemPickupID() == i)
					{
						temp->SetHasPickedUp(loadGameInstance->itemPickupArray[i]);
						if (!temp->GetHasBeenPickedUp())
						{
							temp->EnableLight();
						}
					}
				}
			}
		}
	}
}

void AOverworldPawn::RefreshAllStarGates()
{
	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

	TArray<AActor*> objectArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStarGate::StaticClass(), objectArray);

	if (loadGameInstance)
	{
		for (int i = 0; i < loadGameInstance->starGateArray.Num(); i++)
		{
			for (int j = 0; j < objectArray.Num(); j++)
			{
				AStarGate* temp = Cast<AStarGate>(objectArray[j]);
				if (temp)
				{
					if (temp->GetStarGateID() == i)
					{
						temp->SetHasBeenCleared(loadGameInstance->starGateArray[i]);
						temp->DoFogCheck(totalStarCount);
					}
				}
			}
		}
	}
}

// Called every frame
void AOverworldPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (upInputButton > 0 || upInputButton < 0)
	{
		upInput = upInputButton;
	}
	if (rightInputButton > 0 || rightInputButton < 0)
	{
		rightInput = rightInputButton;
	}

	if (upInput > 0.1 || upInput < -0.1)
	{
		lunaPetAnimInstance->isRunning = true;
		if (!isInJunction)
		{
			if (upInput > 0.1)
			{
				// Get spline tangent direction
				FVector currentSplineTangent = currentSpline->GetTangentAtDistanceAlongSpline(distanceTraveledOnCurrentSpline, ESplineCoordinateSpace::World);
				// Normalize the vector
				currentSplineTangent.Normalize();
				// Get world up vector
				FVector worldVector = FVector(0, -1, 0);
				// Normalize the vector
				worldVector.Normalize();

				// Do movement
				lastMovePositive = true;
				DoMovement(currentSplineTangent, worldVector);
			}
			else if (upInput < -0.1)
			{
				// Get spline tangent direction
				FVector currentSplineTangent = currentSpline->GetTangentAtDistanceAlongSpline(distanceTraveledOnCurrentSpline, ESplineCoordinateSpace::World);
				// Normalize the vector
				currentSplineTangent.Normalize();

				// Get world down vector
				FVector worldVector = FVector(0, 1, 0);
				// Normalize the vector
				worldVector.Normalize();

				// Do movement
				lastMovePositive = false;
				DoMovement(currentSplineTangent, worldVector);
			}
		}
		else
		{
			if (upInput > 0.1)
			{
				if (junctionHasUp && !isInterpolatingToJunctionSpline)
				{
					if (junctionEntranceUp)
					{
						targetIsEntranceSpline = true;
					}
					else
					{
						targetIsEntranceSpline = false;
					}
					isInterpolatingToJunctionSpline = true;
					junctionTargetSpline = junctionUpSpline;
					DoInterpolateToSpline(junctionTargetSpline);
				}
				else if (junctionHasUp && isInterpolatingToJunctionSpline)
				{
					if (junctionTargetSpline == junctionUpSpline)
					{
						DoInterpolateToSpline(junctionTargetSpline);
					}
					else
					{
						if (junctionEntranceUp)
						{
							targetIsEntranceSpline = true;
						}
						else
						{
							targetIsEntranceSpline = false;
						}
						junctionTargetSpline = junctionUpSpline;
						DoInterpolateToSpline(junctionTargetSpline);
					}
				}
			}
			else if (upInput < -0.1)
			{
				if (junctionHasDown && !isInterpolatingToJunctionSpline)
				{
					if (junctionEntranceDown)
					{
						targetIsEntranceSpline = true;
					}
					else
					{
						targetIsEntranceSpline = false;
					}
					isInterpolatingToJunctionSpline = true;
					junctionTargetSpline = junctionDownSpline;
					DoInterpolateToSpline(junctionTargetSpline);
				}
				else if (junctionHasDown && isInterpolatingToJunctionSpline)
				{
					if (junctionTargetSpline == junctionDownSpline)
					{
						DoInterpolateToSpline(junctionTargetSpline);
					}
					else
					{
						if (junctionEntranceDown)
						{
							targetIsEntranceSpline = true;
						}
						else
						{
							targetIsEntranceSpline = false;
						}
						junctionTargetSpline = junctionDownSpline;
						DoInterpolateToSpline(junctionTargetSpline);
					}
				}
			}
		}
	}
	else if (rightInput > 0.1 || rightInput < -0.1)
	{
		lunaPetAnimInstance->isRunning = true;
		if (!isInJunction)
		{
			if (rightInput > 0.1)
			{
				// Get spline tangent direction
				FVector currentSplineTangent = currentSpline->GetTangentAtDistanceAlongSpline(distanceTraveledOnCurrentSpline, ESplineCoordinateSpace::World);
				// Normalize the vector
				currentSplineTangent.Normalize();
				// Get world right vector
				FVector worldVector = FVector(1, 0, 0);
				// Normalize the vector
				worldVector.Normalize();

				// Do movement
				lastMovePositive = true;
				DoMovement(currentSplineTangent, worldVector);
			}
			else if (rightInput < -0.1)
			{
				// Get spline tangent direction
				FVector currentSplineTangent = currentSpline->GetTangentAtDistanceAlongSpline(distanceTraveledOnCurrentSpline, ESplineCoordinateSpace::World);
				// Normalize the vector
				currentSplineTangent.Normalize();

				// Get world left vector
				FVector worldVector = FVector(-1, 0, 0);
				// Normalize the vector
				worldVector.Normalize();

				// Do movement
				lastMovePositive = false;
				DoMovement(currentSplineTangent, worldVector);
			}
		}
		else
		{
			if (rightInput > 0.1)
			{
				if (junctionHasRight && !isInterpolatingToJunctionSpline)
				{
					if (junctionEntranceRight)
					{
						targetIsEntranceSpline = true;
					}
					else
					{
						targetIsEntranceSpline = false;
					}
					isInterpolatingToJunctionSpline = true;
					junctionTargetSpline = junctionRightSpline;
					DoInterpolateToSpline(junctionTargetSpline);
				}
				else if (junctionHasRight && isInterpolatingToJunctionSpline)
				{
					if (junctionTargetSpline == junctionRightSpline)
					{
						DoInterpolateToSpline(junctionTargetSpline);
					}
					else
					{
						if (junctionEntranceRight)
						{
							targetIsEntranceSpline = true;
						}
						else
						{
							targetIsEntranceSpline = false;
						}
						junctionTargetSpline = junctionRightSpline;
						DoInterpolateToSpline(junctionTargetSpline);
					}
				}
			}
			else if (rightInput < -0.1)
			{
				if (junctionHasLeft && !isInterpolatingToJunctionSpline)
				{
					if (junctionEntranceLeft)
					{
						targetIsEntranceSpline = true;
					}
					else
					{
						targetIsEntranceSpline = false;
					}
					isInterpolatingToJunctionSpline = true;
					junctionTargetSpline = junctionLeftSpline;
					DoInterpolateToSpline(junctionTargetSpline);
				}
				else if (junctionHasLeft && isInterpolatingToJunctionSpline)
				{
					if (junctionTargetSpline == junctionLeftSpline)
					{
						DoInterpolateToSpline(junctionTargetSpline);
					}
					else
					{
						if (junctionEntranceLeft)
						{
							targetIsEntranceSpline = true;
						}
						else
						{
							targetIsEntranceSpline = false;
						}
						junctionTargetSpline = junctionLeftSpline;
						DoInterpolateToSpline(junctionTargetSpline);
					}
				}
			}
		}
	}
	else
	{
		lunaPetAnimInstance->isRunning = false;
	}
}

// Called to bind functionality to input
void AOverworldPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("WorldMapMoveUp", this, &AOverworldPawn::MoveUp);
	PlayerInputComponent->BindAxis("WorldMapMoveRight", this, &AOverworldPawn::MoveRight);

	PlayerInputComponent->BindAction("OverworldAction", IE_Pressed ,this, &AOverworldPawn::DoCommitToLevel);
	PlayerInputComponent->BindAction("OverworldAction", IE_Pressed, this, &AOverworldPawn::DoItemPickup);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AOverworldPawn::SaveAndReturnToMenu);

}

void AOverworldPawn::SetUpInput(float input)
{
	upInputButton = input;
}

void AOverworldPawn::SetRightInput(float input)
{
	rightInputButton = input;
}

void AOverworldPawn::DoCommitFromUI()
{
	DoCommitToLevel();
	DoItemPickup();
}

float AOverworldPawn::GetDistanceTraveledOnSpline(USplineComponent* spline)
{

	float inputKey = spline->FindInputKeyClosestToWorldLocation(lunaPet->GetComponentLocation());

	int inputKeyTrunc1 = FMath::TruncToInt(inputKey);
	int inputKeyTrunc2 = FMath::TruncToInt(inputKey + 1.f);

	float distanceOnSpline1 = spline->GetDistanceAlongSplineAtSplinePoint(inputKeyTrunc1);
	float distanceOnSpline2 = spline->GetDistanceAlongSplineAtSplinePoint(inputKeyTrunc2);

	float distanceDelta = distanceOnSpline2 - distanceOnSpline1;
	float keyDelta = inputKey - (float)inputKeyTrunc1;

	float distanceToPoint2 = distanceDelta * keyDelta;

	return (distanceOnSpline1 + distanceToPoint2);
}

void AOverworldPawn::MoveUp(float inputValue)
{
	upInput = inputValue;
}

void AOverworldPawn::MoveRight(float inputValue)
{
	rightInput = inputValue;
}

void AOverworldPawn::DoInterpolateToSpline(USplineComponent* target)
{
	if (target)
	{
		if (targetIsEntranceSpline)
		{
			FVector targetLocation = target->GetLocationAtDistanceAlongSpline(target->GetSplineLength() - 100, ESplineCoordinateSpace::World);
			FVector targetDirection = targetLocation - lunaPet->GetComponentLocation();

			float worldX = FMath::FInterpConstantTo(lunaPet->GetComponentLocation().X, targetLocation.X, GetWorld()->GetDeltaSeconds(), 1000);
			float worldY = FMath::FInterpConstantTo(lunaPet->GetComponentLocation().Y, targetLocation.Y, GetWorld()->GetDeltaSeconds(), 1000);
			lunaPet->SetWorldLocation(FVector(worldX, worldY, lunaPet->GetComponentLocation().Z));
			lunaPet->SetWorldRotation(targetDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).Rotation());
		}
		else
		{
			FVector targetLocation = target->GetLocationAtDistanceAlongSpline(100, ESplineCoordinateSpace::World);
			FVector targetDirection = targetLocation - lunaPet->GetComponentLocation();

			float worldX = FMath::FInterpConstantTo(lunaPet->GetComponentLocation().X, targetLocation.X, GetWorld()->GetDeltaSeconds(), 1000);
			float worldY = FMath::FInterpConstantTo(lunaPet->GetComponentLocation().Y, targetLocation.Y, GetWorld()->GetDeltaSeconds(), 1000);
			lunaPet->SetWorldLocation(FVector(worldX, worldY, lunaPet->GetComponentLocation().Z));
			lunaPet->SetWorldRotation(targetDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).Rotation());
		}
	}
}

void AOverworldPawn::DoMovement(FVector &splineTangentDirection, FVector &desiredMoveDirection)
{
	doingMove = true;
	// Get angle between spline tangent and move direction
	float angleBetween = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(desiredMoveDirection, splineTangentDirection)));

	if (angleBetween < 90 && lastMovePositive)
	{
		isOverrunReturning = false;
		if ((distanceTraveledOnCurrentSpline + moveAmount < currentSpline->GetSplineLength()))
		{
			if (!isStarGateBlocked)
			{
				float targetDistance = distanceTraveledOnCurrentSpline + moveAmount;
				FVector targetRotation = currentSpline->GetTangentAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
				FVector targetLocation = currentSpline->GetLocationAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
				lunaPet->SetWorldLocationAndRotation(targetLocation, targetRotation.RotateAngleAxis(-90, FVector(0,0,1)).Rotation());
				distanceTraveledOnCurrentSpline = targetDistance;
			}
		}
	}
	else if (angleBetween > 90 && !lastMovePositive)
	{
		isOverrunReturning = false;
		if ((distanceTraveledOnCurrentSpline - moveAmount) > 0)
		{
			float targetDistance = distanceTraveledOnCurrentSpline - moveAmount;
			FVector targetRotation = currentSpline->GetTangentAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
			FVector targetLocation = currentSpline->GetLocationAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
			lunaPet->SetWorldLocationAndRotation(targetLocation, targetRotation.RotateAngleAxis(90, FVector(0, 0, 1)).Rotation());
			distanceTraveledOnCurrentSpline = targetDistance;
		}
	}
	else if (angleBetween >= 90 && lastMovePositive)
	{
		if (angleBetween <= 140 && !isOverrunReturning)
		{
			if ((distanceTraveledOnCurrentSpline + moveAmount < currentSpline->GetSplineLength()))
			{
				if (!isStarGateBlocked)
				{
					float targetDistance = distanceTraveledOnCurrentSpline + moveAmount;
					FVector targetRotation = currentSpline->GetTangentAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
					FVector targetLocation = currentSpline->GetLocationAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
					lunaPet->SetWorldLocationAndRotation(targetLocation, targetRotation.RotateAngleAxis(-90, FVector(0, 0, 1)).Rotation());
					distanceTraveledOnCurrentSpline = targetDistance;
				}
			}
		}
		else if (angleBetween > 140 && !isOverrunReturning)
		{
			isOverrunReturning = true;
			if ((distanceTraveledOnCurrentSpline - moveAmount) > 0)
			{
				float targetDistance = distanceTraveledOnCurrentSpline - moveAmount;
				FVector targetRotation = currentSpline->GetTangentAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
				FVector targetLocation = currentSpline->GetLocationAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
				lunaPet->SetWorldLocationAndRotation(targetLocation, targetRotation.RotateAngleAxis(90, FVector(0, 0, 1)).Rotation());
				distanceTraveledOnCurrentSpline = targetDistance;
			}
		}
		else if (isOverrunReturning)
		{
			if ((distanceTraveledOnCurrentSpline - moveAmount) > 0)
			{
				float targetDistance = distanceTraveledOnCurrentSpline - moveAmount;
				FVector targetRotation = currentSpline->GetTangentAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
				FVector targetLocation = currentSpline->GetLocationAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
				lunaPet->SetWorldLocationAndRotation(targetLocation, targetRotation.RotateAngleAxis(90, FVector(0, 0, 1)).Rotation());
				distanceTraveledOnCurrentSpline = targetDistance;
			}
		}
	}
	else if (angleBetween <= 90 && !lastMovePositive)
	{
		if (angleBetween >= 40 && !isOverrunReturning)
		{
			if ((distanceTraveledOnCurrentSpline - moveAmount) > 0)
			{
				float targetDistance = distanceTraveledOnCurrentSpline - moveAmount;
				FVector targetRotation = currentSpline->GetTangentAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
				FVector targetLocation = currentSpline->GetLocationAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
				lunaPet->SetWorldLocationAndRotation(targetLocation, targetRotation.RotateAngleAxis(90, FVector(0, 0, 1)).Rotation());
				distanceTraveledOnCurrentSpline = targetDistance;
			}
		}
		else if (angleBetween < 40 && !isOverrunReturning)
		{
			isOverrunReturning = true;
			if ((distanceTraveledOnCurrentSpline + moveAmount < currentSpline->GetSplineLength()))
			{
				if (!isStarGateBlocked)
				{
					float targetDistance = distanceTraveledOnCurrentSpline + moveAmount;
					FVector targetRotation = currentSpline->GetTangentAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
					FVector targetLocation = currentSpline->GetLocationAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
					lunaPet->SetWorldLocationAndRotation(targetLocation, targetRotation.RotateAngleAxis(-90, FVector(0, 0, 1)).Rotation());
					distanceTraveledOnCurrentSpline = targetDistance;
				}
			}
		}
		else if (isOverrunReturning)
		{
			if ((distanceTraveledOnCurrentSpline + moveAmount < currentSpline->GetSplineLength()))
			{
				if (!isStarGateBlocked)
				{
					float targetDistance = distanceTraveledOnCurrentSpline + moveAmount;
					FVector targetRotation = currentSpline->GetTangentAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
					FVector targetLocation = currentSpline->GetLocationAtDistanceAlongSpline(targetDistance, ESplineCoordinateSpace::World);
					lunaPet->SetWorldLocationAndRotation(targetLocation, targetRotation.RotateAngleAxis(-90, FVector(0, 0, 1)).Rotation());
					distanceTraveledOnCurrentSpline = targetDistance;
				}
			}
		}
	}
}

void AOverworldPawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass() == AJunction::StaticClass())
	{
		isInJunction = true;
		currentJunction = Cast<AJunction>(OtherActor);

		currentJunction->GetTraversableDirections(junctionHasUp, junctionHasLeft, junctionHasDown, junctionHasRight);
		currentJunction->GetEntranceDirection(junctionEntranceUp, junctionEntranceLeft, junctionEntranceDown, junctionEntranceRight);

		if (junctionHasUp)
		{
			if (junctionEntranceUp)
			{
				if (!currentJunction->EntranceHasBeenSet())
				{
					currentJunction->SetUpSpline(currentJunction->GetUpJunction()->GetDownSpline());
					currentJunction->SetEntrance();
				}
			}
			junctionUpSpline = currentJunction->GetUpSpline();
		}
		if (junctionHasLeft)
		{
			if (junctionEntranceLeft)
			{
				if (!currentJunction->EntranceHasBeenSet())
				{
					currentJunction->SetLeftSpline(currentJunction->GetLeftJunction()->GetRightSpline());
					currentJunction->SetEntrance();
				}
			}
			junctionLeftSpline = currentJunction->GetLeftSpline();
		}
		if (junctionHasDown)
		{
			if (junctionEntranceDown)
			{
				if (!currentJunction->EntranceHasBeenSet())
				{
					currentJunction->SetDownSpline(currentJunction->GetDownJunction()->GetUpSpline());
					currentJunction->SetEntrance();
				}
			}
			junctionDownSpline = currentJunction->GetDownSpline();
		}
		if (junctionHasRight)
		{
			if (junctionEntranceRight)
			{
				if (!currentJunction->EntranceHasBeenSet())
				{
					currentJunction->SetRightSpline(currentJunction->GetRightJunction()->GetLeftSpline());
					currentJunction->SetEntrance();
				}
			}
			junctionRightSpline = currentJunction->GetRightSpline();
		}
	}
	else if (OtherActor->GetClass() == ALevelPrompt::StaticClass())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, FString("Overlap start with level prompt"));
		isLevelPrompted = true;
		currentLevelPrompt = Cast<ALevelPrompt>(OtherActor);
		UIWidget->ShowLevelPrompt(currentLevelPrompt);
	}
	else if (OtherActor->GetClass() == AStarGate::StaticClass())
	{
		AStarGate* tempStarGate = Cast<AStarGate>(OtherActor);
		if (tempStarGate)
		{
			if (OtherComp->GetName() == "Query overlap")
			{
				UIWidget->ShowStarGatePrompt(tempStarGate);
				isInStarGate = true;
			}
			else if (OtherComp->GetName() == "Blocking overlap")
			{
				if (!tempStarGate->GetIsBossGate())
				{
					if (totalStarCount < tempStarGate->GetStarRequirement())
					{
						isStarGateBlocked = true;
					}
					else
					{
						if (!tempStarGate->GetHasBeenCleared())
						{
							DoStarGateUnblock(tempStarGate->GetStarGateID());
							tempStarGate->SetHasBeenCleared(true);
							tempStarGate->DoFogCheck(totalStarCount);
						}
						isStarGateBlocked = false;
					}
				}
				else
				{
					if (totalBossKeys < tempStarGate->GetBossKeyRequirment())
					{
						isStarGateBlocked = true;
					}
					else
					{
						if (!tempStarGate->GetHasBeenCleared())
						{
							DoStarGateUnblock(tempStarGate->GetStarGateID());
							tempStarGate->SetHasBeenCleared(true);
							tempStarGate->DoFogCheck(totalBossKeys);
						}
						isStarGateBlocked = false;
					}
				}
			}
		}
	}
	else if (OtherActor->GetClass() == AItemPickup::StaticClass())
	{
		currentItemPickup = Cast<AItemPickup>(OtherActor);
		if (currentItemPickup)
		{
			if (!currentItemPickup->GetHasBeenPickedUp())
			{
				UIWidget->ShowItemPickupPrompt(currentItemPickup);
				isInItemPickup = true;
			}
		}
	}
}

void AOverworldPawn::DoStarGateUnblock(int starGateID)
{
	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));
	loadGameInstance->starGateArray[starGateID] = true;

	UGameplayStatics::SaveGameToSlot(loadGameInstance, localSaveSlotName, loadGameInstance->UserIndex);

	fogClearedParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fogClearedParticleSystem, lunaPet->GetComponentLocation());
}

void AOverworldPawn::OnOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (otherActor->GetClass() == AJunction::StaticClass())
	{
		if (isInJunction)
		{
			if (targetIsEntranceSpline)
			{
				isInJunction = false;
				isInterpolatingToJunctionSpline = false;
				lunaPet->SetWorldLocation(junctionTargetSpline->GetLocationAtDistanceAlongSpline(junctionTargetSpline->GetSplineLength() - 50, ESplineCoordinateSpace::World));
				currentSpline = junctionTargetSpline;
				targetIsEntranceSpline = false;
				distanceTraveledOnCurrentSpline = currentSpline->GetSplineLength() - 50;
			}
			else
			{
				isInJunction = false;
				isInterpolatingToJunctionSpline = false;
				lunaPet->SetWorldLocation(junctionTargetSpline->GetLocationAtDistanceAlongSpline(50, ESplineCoordinateSpace::World));
				currentSpline = junctionTargetSpline;
				targetIsEntranceSpline = false;
				distanceTraveledOnCurrentSpline = 50;
			}
		}
	}
	else if (otherActor->GetClass() == ALevelPrompt::StaticClass())
	{
		if (isLevelPrompted)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, FString("Overlap ended with level prompt"));
			isLevelPrompted = false;
			UIWidget->HideLevelPrompt();
		}
	}
	else if (otherActor->GetClass() == AStarGate::StaticClass())
	{
		if (isInStarGate)
		{
			UIWidget->HideStarGatePrompt();
			isInStarGate = false;
			isStarGateBlocked = false;
		}
	}
	else if (otherActor->GetClass() == AItemPickup::StaticClass())
	{
		if (isInItemPickup)
		{
			isInItemPickup = false;
			UIWidget->HideItemPickupPrompt();
		}
	}
}

void AOverworldPawn::UpdateGameInstanceForCampaignLevel(int fCode, int sIndex, int toRead, int qWin, int qLose, int wMode, int qMode)
{
	gameInstance->fileCode = fCode;
	gameInstance->isFreePlay = false;
	gameInstance->startIndex = sIndex;
	gameInstance->totalQuestionsToRead = toRead;
	gameInstance->minQuestionsToWin = qWin;
	gameInstance->minQuestionsToLose = qLose;
	gameInstance->wordTypeMode = wMode;
	gameInstance->questionMode = qMode;
	gameInstance->savedLevelPromptID = currentLevelPrompt->levelID;
	gameInstance->autoFeverHeld = totalAutoFeverHeld;
	gameInstance->questionReduceHeld = totalQuestionReduceHeld;
	gameInstance->supportHeld = totalSupportHeld;

	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

	loadGameInstance->savedCharacterDistanceOnSpline = distanceTraveledOnCurrentSpline;
	loadGameInstance->savedJunctionID = currentJunction->GetJunctionID();
	loadGameInstance->savedLevelPromptID = currentLevelPrompt->levelID;
	loadGameInstance->playerTotalAutoFeverHeld = totalAutoFeverHeld;
	loadGameInstance->playerTotalQuestionReduceHeld = totalQuestionReduceHeld;
	loadGameInstance->playerTotalSupportHeld = totalSupportHeld;

	gameInstance->levelID = loadGameInstance->levelID;

	if (currentSpline == currentJunction->GetUpSpline())
	{
		loadGameInstance->savedSplineID = 0;
	}
	else if (currentSpline == currentJunction->GetLeftSpline())
	{
		loadGameInstance->savedSplineID = 1;
	}
	else if (currentSpline == currentJunction->GetDownSpline())
	{
		loadGameInstance->savedSplineID = 2;
	}
	else if (currentSpline == currentJunction->GetRightSpline())
	{
		loadGameInstance->savedSplineID = 3;
	}

	UGameplayStatics::SaveGameToSlot(loadGameInstance, localSaveSlotName, loadGameInstance->UserIndex);
}

void AOverworldPawn::CommitToGameInstance()
{
	UpdateGameInstanceForCampaignLevel(currentLevelPrompt->fileCode, currentLevelPrompt->startIndex, currentLevelPrompt->totalQuestionsToRead, currentLevelPrompt->minQuestionsToWin, currentLevelPrompt->minQuestionsToLose, currentLevelPrompt->wordTypeMode, currentLevelPrompt->questionMode);
	UIWidget->DoTransitionToLevel();
}

void AOverworldPawn::DoCommitToLevel()
{
	if (isLevelPrompted)
	{
		UpdateGameInstanceForCampaignLevel(currentLevelPrompt->fileCode, currentLevelPrompt->startIndex, currentLevelPrompt->totalQuestionsToRead, currentLevelPrompt->minQuestionsToWin, currentLevelPrompt->minQuestionsToLose, currentLevelPrompt->wordTypeMode, currentLevelPrompt->questionMode);
		UIWidget->DoTransitionToLevel();
	}
}

void AOverworldPawn::DoItemPickup()
{
	if (isInItemPickup)
	{
		currentItemPickup->DoPlayPickupParticles();
		totalAutoFeverHeld += currentItemPickup->GetAutoFeverCount();
		totalQuestionReduceHeld += currentItemPickup->GetQuestionReduceCount();
		totalSupportHeld += currentItemPickup->GetSupportCount();
		currentItemPickup->SetHasPickedUp(true);
		UIWidget->UpdatePowerupCount(totalAutoFeverHeld, totalQuestionReduceHeld, totalSupportHeld);
		isInItemPickup = false;
		UIWidget->HideItemPickupPrompt();

		UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
		loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

		if (loadGameInstance)
		{
			loadGameInstance->itemPickupArray[currentItemPickup->GetItemPickupID()] = true;
			UGameplayStatics::SaveGameToSlot(loadGameInstance, localSaveSlotName, loadGameInstance->UserIndex);
		}
	}
}

void AOverworldPawn::SetCurrentSpline(USplineComponent* sp, int ID)
{
	currentSpline = sp;
}

void AOverworldPawn::SaveAndReturnToMenu()
{
	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

	// Get all levelPrompts in the world
	TArray<AActor*> levelPromptArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelPrompt::StaticClass(), levelPromptArray);

	if (loadGameInstance)
	{
		// Update all levelPrompts in save file
		for (int i = 0; i < loadGameInstance->levelPromptArray.Num(); i++)
		{
			for (int j = 0; j < levelPromptArray.Num(); j++)
			{
				ALevelPrompt* lvlPrompt = Cast<ALevelPrompt>(levelPromptArray[j]);
				if (lvlPrompt)
				{
					if (lvlPrompt->levelID == i)
					{
						loadGameInstance->levelPromptArray[i].completed = lvlPrompt->hasBeenCompleted;
						loadGameInstance->levelPromptArray[i].score = lvlPrompt->completedScore;
						loadGameInstance->levelPromptArray[i].questionsCorrect = lvlPrompt->completedQuestionsCorrect;
						loadGameInstance->levelPromptArray[i].starsEarned = lvlPrompt->starsEarned;
					}
				}
			}
		}
		// Save character current position
		// Save charater current spline distance
		loadGameInstance->savedCharacterDistanceOnSpline = distanceTraveledOnCurrentSpline;
		// Save character current junction (allows for getting current spline)
		loadGameInstance->savedJunctionID = currentJunction->GetJunctionID();
		// Save character current splineID
		if (currentSpline == currentJunction->GetUpSpline())
		{
			loadGameInstance->savedSplineID = 0;
		}
		else if (currentSpline == currentJunction->GetLeftSpline())
		{
			loadGameInstance->savedSplineID = 1;
		}
		else if (currentSpline == currentJunction->GetDownSpline())
		{
			loadGameInstance->savedSplineID = 2;
		}
		else if (currentSpline == currentJunction->GetRightSpline())
		{
			loadGameInstance->savedSplineID = 3;
		}

		loadGameInstance->playerTotalStars = totalStarCount;
		loadGameInstance->playerTotalAutoFeverHeld = totalAutoFeverHeld;
		loadGameInstance->playerTotalQuestionReduceHeld = totalQuestionReduceHeld;
		loadGameInstance->playerTotalSupportHeld = totalSupportHeld;

		// Save out game
		UGameplayStatics::SaveGameToSlot(loadGameInstance, localSaveSlotName, loadGameInstance->UserIndex);
		UIWidget->ReturnToMenu();
	}
}

void AOverworldPawn::TransitionToLevel(int levelID, int previousLevelID)
{
	UTrampolineSaveGame* loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadGameInstance = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(localSaveSlotName, loadGameInstance->UserIndex));

	// Get all levelPrompts in the world
	TArray<AActor*> levelPromptArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelPrompt::StaticClass(), levelPromptArray);

	if (loadGameInstance)
	{
		// Update all levelPrompts in save file
		for (int i = 0; i < loadGameInstance->levelPromptArray.Num(); i++)
		{
			for (int j = 0; j < levelPromptArray.Num(); j++)
			{
				ALevelPrompt* lvlPrompt = Cast<ALevelPrompt>(levelPromptArray[j]);
				if (lvlPrompt)
				{
					if (lvlPrompt->levelID == i)
					{
						loadGameInstance->levelPromptArray[i].completed = lvlPrompt->hasBeenCompleted;
						loadGameInstance->levelPromptArray[i].score = lvlPrompt->completedScore;
						loadGameInstance->levelPromptArray[i].questionsCorrect = lvlPrompt->completedQuestionsCorrect;
						loadGameInstance->levelPromptArray[i].starsEarned = lvlPrompt->starsEarned;
					}
				}
			}
		}
		// Save character current position
		// Save charater current spline distance
		loadGameInstance->savedCharacterDistanceOnSpline = distanceTraveledOnCurrentSpline;
		// Save character current junction (allows for getting current spline)
		loadGameInstance->savedJunctionID = currentJunction->GetJunctionID();
		// Save character current splineID
		if (currentSpline == currentJunction->GetUpSpline())
		{
			loadGameInstance->savedSplineID = 0;
		}
		else if (currentSpline == currentJunction->GetLeftSpline())
		{
			loadGameInstance->savedSplineID = 1;
		}
		else if (currentSpline == currentJunction->GetDownSpline())
		{
			loadGameInstance->savedSplineID = 2;
		}
		else if (currentSpline == currentJunction->GetRightSpline())
		{
			loadGameInstance->savedSplineID = 3;
		}

		loadGameInstance->playerTotalStars = totalStarCount;
		loadGameInstance->playerTotalAutoFeverHeld = totalAutoFeverHeld;
		loadGameInstance->playerTotalQuestionReduceHeld = totalQuestionReduceHeld;
		loadGameInstance->playerTotalSupportHeld = totalSupportHeld;
		loadGameInstance->levelID = levelID;
		loadGameInstance->previousLevelID = previousLevelID;

		gameInstance->cameFromOverworldLevel = true;
		gameInstance->cameFromLevel = false;
		// Save out game
		UGameplayStatics::SaveGameToSlot(loadGameInstance, localSaveSlotName, loadGameInstance->UserIndex);
		UIWidget->DoTransitionToOverworldLevel(levelID);
	}
}

void AOverworldPawn::GetLevelID()
{

}

bool AOverworldPawn::GetCameFromOverworldLevel()
{
	return gameInstance->cameFromLevel;
}