// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogSpawner.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AFogSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFogSpawner();

	UParticleSystem* fogParticleSystem;
	class UParticleSystemComponent* fogParticleSystemComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;
	UPROPERTY(EditAnywhere)
	USceneComponent* root;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PlayFogParticles();
	void StopFogParticles();
	
	bool isPlayingParticles = false;
};
