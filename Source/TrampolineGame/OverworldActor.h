// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OverworldActor.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AOverworldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOverworldActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	class UStaticMeshComponent* characterMesh;

	class UCameraComponent* cameraComponent;
	class USpringArmComponent* springArmComponent;

	class USplineComponent* currentSpline;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
