// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameEndTransitionActor.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AGameEndTransitionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameEndTransitionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* overlap;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
