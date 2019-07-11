// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemPickup.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine.h"
#include "ConstructorHelpers.h"


// Sets default values
AItemPickup::AItemPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	overlap = CreateDefaultSubobject<UBoxComponent>("ItemPickupOverlap");
	overlap->SetBoxExtent(FVector(50, 50, 50));
	overlap->AddLocalOffset(FVector(0, 0, 100));
	overlap->SetCollisionObjectType(ECC_GameTraceChannel6);
	overlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	overlap->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	SetRootComponent(overlap);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> objMesh1(TEXT("/Game/Trampoline/Meshes/ItemPickup_NotUsed"));
	notTakenMesh = objMesh1.Object;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> objMesh2(TEXT("/Game/Trampoline/Meshes/ItemPickup_Used"));
	takenMesh = objMesh2.Object;
	mesh->SetStaticMesh(notTakenMesh);
	mesh->SetupAttachment(overlap);
	mesh->SetRelativeLocation(FVector::ZeroVector);

	pickupParticleSystem = CreateDefaultSubobject<UParticleSystem>("Pickup particle system");
	static ConstructorHelpers::FObjectFinder<UParticleSystem> particleSysObj0(TEXT("/Game/Trampoline/Particles/Pickup_ParticleSystem"));
	pickupParticleSystem = particleSysObj0.Object;

	itemPickupLight = CreateDefaultSubobject<UPointLightComponent>("Pickup light");
	itemPickupLight->SetupAttachment(mesh);
	itemPickupLight->SetRelativeLocation(FVector::ZeroVector);
	itemPickupLight->AddLocalOffset(FVector(-5, 0, 130));
	itemPickupLight->SetIntensity(1);
	itemPickupLight->SetAttenuationRadius(300);
	itemPickupLight->CastShadows = false;
	itemPickupLight->IntensityUnits = ELightUnits::Candelas;
	itemPickupLight->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void AItemPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemPickup::SetHasPickedUp(bool pickedup)
{
	if (pickedup)
	{
		hasBeenPickedUp = true;
		mesh->SetStaticMesh(takenMesh);
	}
	else
	{
		hasBeenPickedUp = false;
		mesh->SetStaticMesh(notTakenMesh);
	}
}

void AItemPickup::DoPlayPickupParticles()
{
	pickupParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(pickupParticleSystem, mesh);
}

void AItemPickup::EnableLight()
{
	itemPickupLight->SetHiddenInGame(false);
}
