// Fill out your copyright notice in the Description page of Project Settings.

#include "CinematicPawn.h"
#include "Camera/CameraComponent.h"
#include "TrampolineCinematicWidget.h"
#include "Engine.h"

// Sets default values
ACinematicPawn::ACinematicPawn()
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACinematicPawn::BeginPlay()
{
	Super::BeginPlay();

	UIWidget = Cast<UTrampolineCinematicWidget>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	if (UIWidget)
	{
		UIWidget->AddToViewport();
	}

	OnDoPlayDelegate.AddDynamic(this, &ACinematicPawn::DoPlay);
}

// Called every frame
void ACinematicPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACinematicPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACinematicPawn::ChangeTextIndex(bool forward)
{
	if (forward)
	{
		if (currentTextIndex < cutsceneTextArray.Num()-1)
		{
			currentTextIndex += 1;
			UIWidget->ShowNewText(currentTextIndex);
			OnDoPlayDelegate.Broadcast();
		}
		else
		{
			UIWidget->Transition(end);
		}
	}
	else
	{
		if (currentTextIndex > 0)
		{
			currentTextIndex -= 1;
		}
	}
}

void ACinematicPawn::EnableProgress()
{
	UIWidget->EnableProgression();
}

void ACinematicPawn::DisableProgress()
{
	UIWidget->DisableProgression();
}


void ACinematicPawn::DoPlay()
{

}
