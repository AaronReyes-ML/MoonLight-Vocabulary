// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogSpawner.h"
#include "StarGate.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AStarGate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStarGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USceneComponent* root;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* queryOverlap;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* blockingOverlap;

	UPROPERTY(EditAnywhere)
	int starRequirement = 0;

	UPROPERTY(EditAnywhere)
	int bossKeyRequirment = 0;

	UPROPERTY(EditAnywhere)
	TArray<AFogSpawner*> fogSpawnerArray;

	UPROPERTY(EditAnywhere)
	int starGateID = 0;

	bool hasBeenCleared = false;
	UPROPERTY(EditAnywhere)
	bool isBossGate = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetStarRequirement() { return starRequirement; };

	void DoFogCheck(int starCount);

	void SetHasBeenCleared(bool value);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetStarGateID() { return starGateID; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetHasBeenCleared() { return hasBeenCleared; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsBossGate() { return isBossGate; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetBossKeyRequirment() { return bossKeyRequirment; };
};
