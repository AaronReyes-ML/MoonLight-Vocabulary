// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "TrampolineMenuWidget.h"
#include "TrampolineGameInstance.h"
#include "Engine.h"
#include "TrampolineSaveGame.h"


// Sets default values
AMenuPawn::AMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->bAbsoluteRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);


	// Create an orthographic camera (no perspective) and attach it to the boom
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->bAbsoluteRotation = true;
	camera->bUsePawnControlRotation = false;
	camera->bAutoActivate = true;

}

// Called when the game starts or when spawned
void AMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* menuController = GetWorld()->GetFirstPlayerController();

	gameInstance = Cast<UTrampolineGameInstance>(GetGameInstance());
	gameInstance->cameFromLevel = false;

	menuController->bShowMouseCursor = true;
	menuController->bEnableClickEvents = true;
	menuController->bEnableMouseOverEvents = true;

	UIWidget = Cast<UTrampolineMenuWidget>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	if (UIWidget)
	{
		UIWidget->AddToViewport();
	}
}

// Called every frame
void AMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMenuPawn::UpdateGameInstanceForFreePlay(int fCode, int wMode, int qMode)
{
	gameInstance->fileCode = fCode;
	gameInstance->isFreePlay = true;
	gameInstance->wordTypeMode = wMode;
	gameInstance->questionMode = qMode;
}

void AMenuPawn::CreateGameSave(int index)
{
	UTrampolineSaveGame* initSaveGame = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	for (int i = 0; i < totalLevelsOnStage1; i++)
	{
		initSaveGame->levelPromptArray.Add(FLevelCriticalInformationStruct(false, 0, 0, 0));
	}
	for (int i = 0; i < totalItemPickupsOnStage1; i++)
	{
		initSaveGame->itemPickupArray.Add(false);
	}
	for (int i = 0; i < totalStarGatesOnStage1; i++)
	{
		initSaveGame->starGateArray.Add(false);
	}

	switch (index)
	{
	case 1:
		initSaveGame->SaveSlotName = TEXT("SaveSlot1");
		break;
	case 2:
		initSaveGame->SaveSlotName = TEXT("SaveSlot2");
		break;
	case 3:
		initSaveGame->SaveSlotName = TEXT("SaveSlot3");
		initalStars = 499;
		initialBossKeys = 5;
		break;
	default:
		initSaveGame->SaveSlotName = TEXT("SaveSlot1");
		break;
	}

	gameInstance->instancedSaveSlotName = initSaveGame->SaveSlotName;
	gameInstance->levelID = 0;

	initSaveGame->playerTotalStars = initalStars;
	initSaveGame->playerTotalAutoFeverHeld = 0;
	initSaveGame->playerTotalQuestionReduceHeld = 0;
	initSaveGame->playerTotalSupportHeld = 0;
	initSaveGame->levelID = 0;
	initSaveGame->previousLevelID = -1;
	initSaveGame->levelKeysHeld = initialBossKeys;
	UGameplayStatics::SaveGameToSlot(initSaveGame, initSaveGame->SaveSlotName, initSaveGame->UserIndex);
}

bool AMenuPawn::DoesGameSaveExist()
{
	UTrampolineSaveGame* loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));

	loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot1"),0));
	if (loadedGameSave) 
	{
		return true;
	}
	else
	{
		loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot2"), 0));
		if (loadedGameSave)
		{
			return true;
		}
		else
		{
			loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot3"), 0));
			if (loadedGameSave)
			{
				return true;
			}
		}
	}
	
	return false;
}

int AMenuPawn::GetLevelID(int index)
{
	UTrampolineSaveGame * loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	switch (index)
	{
	case 1:
		loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot1", loadedGameSave->UserIndex));
		if (loadedGameSave)
		return loadedGameSave->levelID;
		break;
	case 2:
		loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot2", loadedGameSave->UserIndex));
		if (loadedGameSave)
		return loadedGameSave->levelID;
		break;
	case 3:
		loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot3", loadedGameSave->UserIndex));
		if (loadedGameSave)
		return loadedGameSave->levelID;
		break;
	default:
		break;
	}
	return 0;
}

int AMenuPawn::GetSaveSlot1Progress()
{
	UTrampolineSaveGame* loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot1", loadedGameSave->UserIndex));

	if (loadedGameSave)
	{
		return loadedGameSave->playerTotalStars;
	}
	else
	{
		return -1;
	}
}

int AMenuPawn::GetSaveSlot2Progress()
{
	UTrampolineSaveGame* loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot2", loadedGameSave->UserIndex));

	if (loadedGameSave)
	{
		return loadedGameSave->playerTotalStars;
	}
	else
	{
		return -1;
	}
}

int AMenuPawn::GetSaveSlot3Progress()
{
	UTrampolineSaveGame* loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));
	loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot3", loadedGameSave->UserIndex));

	if (loadedGameSave)
	{
		return loadedGameSave->playerTotalStars;
	}
	else
	{
		return -1;
	}
}

void AMenuPawn::SetGameInstanceSaveSlotName(FString fileName)
{
	gameInstance->instancedSaveSlotName = fileName;
}