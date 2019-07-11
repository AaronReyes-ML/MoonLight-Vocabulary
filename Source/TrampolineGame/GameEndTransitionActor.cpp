// Fill out your copyright notice in the Description page of Project Settings.

#include "GameEndTransitionActor.h"
#include "Components/SphereComponent.h"


// Sets default values
AGameEndTransitionActor::AGameEndTransitionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void AGameEndTransitionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameEndTransitionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

