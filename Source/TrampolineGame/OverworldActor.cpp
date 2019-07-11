// Fill out your copyright notice in the Description page of Project Settings.

#include "OverworldActor.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine.h"
#include "ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "Components/SplineComponent.h"


// Sets default values
AOverworldActor::AOverworldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void AOverworldActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOverworldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

