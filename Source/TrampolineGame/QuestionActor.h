// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "QuestionActor.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AQuestionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Trampoline
	UPROPERTY(EditAnywhere)
	class USceneComponent* root;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* jumpZone;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* groundZone;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* jumpZoneVisualization;

	UPROPERTY(EditAnywhere)
	USceneComponent* baseParticleEmitterLocation;
	UPROPERTY(EditAnywhere)
	USceneComponent* midParticleEmitterLocation;
	UPROPERTY(EditAnywhere)
	USceneComponent* topParticleEmitterLocation;

	int jumpZoneMaxBoxExtent = 250.f;
	int jumpZoneMinBoxExtent = 50.f;

	int visMaxScale = 5;
	int visMinScale = 1;

	UParticleSystem* baseParticleSystem;
	UParticleSystem* topParticleSystem;
	UParticleSystem* midParticleSystem;

	UParticleSystemComponent* baseParticleSystemComponent;
	UParticleSystemComponent* topParticleSystemComponent;
	UParticleSystemComponent* midParticleSystemComponent;

	UParticleSystemComponent* downwardParticleSystemComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetBoxSize(float percent);
	void SetBoxEnabled(bool emabled);

	void PlayBaseParticleSystem();
	void PlayTopParticleSystem();
	void PlayMidParticleSystem();
	void StopBaseParticleSystem();

	UFUNCTION()
	FORCEINLINE float getBaseLocationZ() { return baseParticleEmitterLocation->GetComponentLocation().Z; };
	UFUNCTION()
	FORCEINLINE float getTopLocationZ() { return topParticleEmitterLocation->GetComponentLocation().Z; };
};
