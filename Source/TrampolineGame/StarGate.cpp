// Fill out your copyright notice in the Description page of Project Settings.

#include "StarGate.h"
#include "Engine.h"
#include "Components/BoxComponent.h"


// Sets default values
AStarGate::AStarGate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>("root");
	SetRootComponent(root);
	root->AddLocalOffset(FVector(0, 0, 100));

	queryOverlap = CreateDefaultSubobject<UBoxComponent>("Query overlap");
	queryOverlap->SetupAttachment(root);
	queryOverlap->SetRelativeLocation(FVector::ZeroVector);
	queryOverlap->SetBoxExtent(FVector(250, 250, 250));
	queryOverlap->SetCollisionObjectType(ECC_GameTraceChannel5);
	queryOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	queryOverlap->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);

	blockingOverlap = CreateDefaultSubobject<UBoxComponent>("Blocking overlap");
	blockingOverlap->SetupAttachment(root);
	blockingOverlap->SetRelativeLocation(FVector::ZeroVector);
	blockingOverlap->SetBoxExtent(FVector(100, 100, 100));
	blockingOverlap->SetCollisionObjectType(ECC_GameTraceChannel4);
	blockingOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	blockingOverlap->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	blockingOverlap->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

// Called when the game starts or when spawned
void AStarGate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStarGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStarGate::DoFogCheck(int starCount)
{
	if (!isBossGate)
	{
		for (int i = 0; i < fogSpawnerArray.Num(); i++)
		{
			AFogSpawner* temp = fogSpawnerArray[i];
			if (starCount < starRequirement)
			{
				temp->PlayFogParticles();
				temp->isPlayingParticles = true;
			}
			else if (starCount >= starRequirement && !hasBeenCleared)
			{
				temp->PlayFogParticles();
				temp->isPlayingParticles = true;
			}
			else if (hasBeenCleared)
			{
				if (temp->isPlayingParticles)
				{
					temp->StopFogParticles();
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < fogSpawnerArray.Num(); i++)
		{
			AFogSpawner* temp = fogSpawnerArray[i];
			if (starCount < bossKeyRequirment)
			{
				temp->PlayFogParticles();
				temp->isPlayingParticles = true;
			}
			else if (starCount >= bossKeyRequirment && !hasBeenCleared)
			{
				temp->PlayFogParticles();
				temp->isPlayingParticles = true;
			}
			else if (hasBeenCleared)
			{
				if (temp->isPlayingParticles)
				{
					temp->StopFogParticles();
				}
			}
		}
	}
}

void AStarGate::SetHasBeenCleared(bool val)
{
	hasBeenCleared = val;
}
