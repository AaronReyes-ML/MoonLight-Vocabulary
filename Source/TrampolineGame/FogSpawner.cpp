// Fill out your copyright notice in the Description page of Project Settings.

#include "FogSpawner.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine.h"
#include "ConstructorHelpers.h"

// Sets default values
AFogSpawner::AFogSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(root);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> objMesh0(TEXT("/Game/Meshes/Sphere"));
	mesh->SetStaticMesh(objMesh0.Object);
	mesh->SetupAttachment(RootComponent);
	mesh->SetRelativeLocation(FVector::ZeroVector);
	mesh->SetHiddenInGame(true);

	fogParticleSystem = CreateDefaultSubobject<UParticleSystem>("Fog particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> pSO1(TEXT("/Game/Trampoline/Particles/Fog_ParticelSystem"));
	fogParticleSystem = pSO1.Object;
}

// Called when the game starts or when spawned
void AFogSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFogSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFogSpawner::PlayFogParticles()
{
	fogParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(fogParticleSystem, mesh);
	mesh->SetHiddenInGame(true);
	isPlayingParticles = true;
}

void AFogSpawner::StopFogParticles()
{
	if (isPlayingParticles)
	{
		fogParticleSystemComponent->ResetToDefaults();
		mesh->SetHiddenInGame(true);
	}
}

