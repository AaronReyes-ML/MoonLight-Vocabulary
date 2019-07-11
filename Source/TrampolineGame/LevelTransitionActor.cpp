// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelTransitionActor.h"
#include "Components/SphereComponent.h"
#include "OverworldPawn.h"


// Sets default values
ALevelTransitionActor::ALevelTransitionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	levelTransitionOverlap = CreateDefaultSubobject <USphereComponent> ("Level transition overlap");
	levelTransitionOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	levelTransitionOverlap->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	levelTransitionOverlap->SetCollisionEnabled(ECollisionEnabled::QueryOnly);


}

// Called when the game starts or when spawned
void ALevelTransitionActor::BeginPlay()
{
	Super::BeginPlay();

	levelTransitionOverlap->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionActor::OnOverlapBegin);
}

// Called every frame
void ALevelTransitionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTransitionActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (player)
	{
		player->TransitionToLevel(targetLevel, currentLevel);
	}
}

