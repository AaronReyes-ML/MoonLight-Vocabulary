// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelPrompt.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ConstructorHelpers.h"
#include "Engine.h"


// Sets default values
ALevelPrompt::ALevelPrompt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(root);
	root->SetMobility(EComponentMobility::Static);
	
	levelPromptOverlap = CreateDefaultSubobject<USphereComponent>("Level overlap");
	levelPromptOverlap->SetupAttachment(RootComponent);
	levelPromptOverlap->AddLocalOffset(FVector(0, 0, 100));
	levelPromptOverlap->SetWorldRotation(FVector(0, -1, 0).Rotation());
	levelPromptOverlap->SetSphereRadius(100.f);
	levelPromptOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	levelPromptOverlap->SetCollisionObjectType(ECC_GameTraceChannel3);
	levelPromptOverlap->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);

	levelPromptSpotLight = CreateDefaultSubobject<USpotLightComponent>("Spot light");
	levelPromptSpotLight->SetupAttachment(RootComponent);
	levelPromptSpotLight->SetRelativeLocation(FVector(0, 0, 1500));
	levelPromptSpotLight->SetRelativeRotation(FRotator(-90, 0, 0));
	levelPromptSpotLight->SetIntensity(500);
	levelPromptSpotLight->SetOuterConeAngle(16);
	levelPromptSpotLight->SetAttenuationRadius(2003);
	levelPromptSpotLight->IntensityUnits = ELightUnits::Candelas;
	levelPromptSpotLight->SetMobility(EComponentMobility::Static);

	levelPromptParticleSystem = CreateDefaultSubobject<UParticleSystem>("Level prompt particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> pSO0(TEXT("/Game/Trampoline/Particles/CircleTest"));
	levelPromptParticleSystem = pSO0.Object;

	levelPromptLightShaft = CreateDefaultSubobject<UStaticMeshComponent>("Light shaft");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObj0(TEXT("/Game/Trampoline/Meshes/lightshaft_125_125_100_2"));
	levelPromptLightShaft->SetStaticMesh(staticMeshObj0.Object);
	levelPromptLightShaft->SetupAttachment(RootComponent);
	levelPromptLightShaft->SetRelativeLocation(FVector::ZeroVector);
	levelPromptLightShaft->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// Called when the game starts or when spawned
void ALevelPrompt::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALevelPrompt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//
}

void ALevelPrompt::DoPlayLevelPromptParticles()
{
	isPlayingParticles = true;
	levelPromptParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(levelPromptParticleSystem, levelPromptOverlap);
}

void ALevelPrompt::StopLevelPromptParticles()
{
	levelPromptParticleSystemComponent->ResetToDefaults();
}