// Fill out your copyright notice in the Description page of Project Settings.

#include "SplashMenuPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "TrampolineMenuWidget.h"
#include "Engine.h"
#include "TrampolineSaveGame.h"


// Sets default values
ASplashMenuPawn::ASplashMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
void ASplashMenuPawn::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* menuController = GetWorld()->GetFirstPlayerController();

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
void ASplashMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASplashMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Start", IE_Released, this, &ASplashMenuPawn::Start);

}

bool ASplashMenuPawn::DoesGameSaveExist()
{
	UTrampolineSaveGame* loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::CreateSaveGameObject(UTrampolineSaveGame::StaticClass()));

	loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot1"), 0));
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
			loadedGameSave = Cast<UTrampolineSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot1"), 0));
			if (loadedGameSave)
			{
				return true;
			}
		}
	}

	return false;
}

void ASplashMenuPawn::Start()
{
	UIWidget->DoTransitionToLevel();
}