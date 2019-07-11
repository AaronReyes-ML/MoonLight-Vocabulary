// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestionActor.h"
#include "Components/BoxComponent.h"
#include "ConstructorHelpers.h"
#include "Engine.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AQuestionActor::AQuestionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(root);

	jumpZone = CreateDefaultSubobject<UBoxComponent>("Jump zone");
	jumpZone->SetupAttachment(RootComponent);
	jumpZone->SetBoxExtent(FVector(100, 100, 150));
	jumpZone->SetRelativeLocation(FVector::ZeroVector);
	groundZone = CreateDefaultSubobject<UBoxComponent>("Ground zone");
	groundZone->SetupAttachment(RootComponent);
	groundZone->SetBoxExtent(FVector(100, 100, 150));
	groundZone->SetRelativeLocation(FVector(0, 0, -100));
	jumpZoneVisualization = CreateDefaultSubobject<UStaticMeshComponent>("Jump zone visual");
	jumpZoneVisualization->SetupAttachment(RootComponent);
	jumpZoneVisualization->SetRelativeLocation(FVector::ZeroVector);
	jumpZoneVisualization->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	jumpZoneVisualization->SetRelativeScale3D(FVector(2, 2, 3));
	jumpZoneVisualization->SetHiddenInGame(true);

	baseParticleEmitterLocation = CreateDefaultSubobject<USceneComponent>("Base emitter location");
	baseParticleEmitterLocation->SetupAttachment(RootComponent);
	baseParticleEmitterLocation->SetRelativeLocation(FVector::ZeroVector);
	midParticleEmitterLocation = CreateDefaultSubobject<USceneComponent>("Mid emitter location");
	midParticleEmitterLocation->SetupAttachment(RootComponent);
	midParticleEmitterLocation->SetRelativeLocation(FVector(0, 0, 50));
	topParticleEmitterLocation = CreateDefaultSubobject<USceneComponent>("Top emitter location");
	topParticleEmitterLocation->SetupAttachment(RootComponent);
	topParticleEmitterLocation->SetRelativeLocation(FVector(0, 0, 100));

	baseParticleSystem = CreateDefaultSubobject<UParticleSystem>("Base particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> psO0(TEXT("/Game/Trampoline/Particles/MagicCircles/MCircle_Base"));
	baseParticleSystem = psO0.Object;

	topParticleSystem = CreateDefaultSubobject<UParticleSystem>("Top particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> psO1(TEXT("/Game/Trampoline/Particles/MagicCircles/MCircle_Top"));
	topParticleSystem = psO1.Object;

	midParticleSystem = CreateDefaultSubobject<UParticleSystem>("Mid particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> psO2(TEXT("/Game/Trampoline/Particles/MagicCircles/MCircle_Mid"));
	midParticleSystem = psO2.Object;

}

// Called when the game starts or when spawned
void AQuestionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AQuestionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuestionActor::SetBoxSize(float percent)
{
	jumpZone->SetBoxExtent(FVector(100, 100, (jumpZoneMinBoxExtent + (jumpZoneMaxBoxExtent - jumpZoneMinBoxExtent) * percent)));
	jumpZoneVisualization->SetRelativeScale3D(FVector(2, 2, (visMinScale + (visMaxScale - visMinScale) * percent)));
}

void AQuestionActor::SetBoxEnabled(bool enabled)
{
	jumpZoneVisualization->SetHiddenInGame(!enabled);
}

void AQuestionActor::PlayBaseParticleSystem()
{
	baseParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(baseParticleSystem, baseParticleEmitterLocation);
}

void AQuestionActor::PlayTopParticleSystem()
{
	topParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(topParticleSystem, topParticleEmitterLocation);
}

void AQuestionActor::PlayMidParticleSystem()
{
	midParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(midParticleSystem, midParticleEmitterLocation);
}

void AQuestionActor::StopBaseParticleSystem()
{
	midParticleSystemComponent->ResetToDefaults();
}
