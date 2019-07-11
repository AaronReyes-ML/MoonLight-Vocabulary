// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionActor.generated.h"

UCLASS()
class TRAMPOLINEGAME_API ALevelTransitionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelTransitionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int targetLevel = 0;
	UPROPERTY(EditAnywhere)
	int currentLevel = 0;

	UPROPERTY(EditAnywhere)
	class USphereComponent* levelTransitionOverlap;
	UPROPERTY(EditAnywhere)
	class AOverworldPawn* player;

	UPROPERTY(EditAnywhere)
	int entranceSplineIndex = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE int GetTargetLevel() { return targetLevel; };

	FORCEINLINE int GetEndSplineIndex() { return entranceSplineIndex; };

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
