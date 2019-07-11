// Fill out your copyright notice in the Description page of Project Settings.

#include "Junction.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AJunction::AJunction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>("Root");
	root->AddLocalOffset(FVector(0, 0, 100));
	root->SetWorldRotation(FRotator(0, -90, 0));

	upSpline = CreateDefaultSubobject<USplineComponent>("Up Spline");
	upSpline->SetupAttachment(root);
	upSpline->SetRelativeLocation(FVector(125, 0, 0));

	leftSpline = CreateDefaultSubobject<USplineComponent>("Left Spline");
	leftSpline->SetupAttachment(root);
	leftSpline->SetRelativeLocation(FVector(0, -125, 0));
	leftSpline->SetRelativeRotation(FRotator(0, -90, 0));

	downSpline = CreateDefaultSubobject<USplineComponent>("Down Spline");
	downSpline->SetupAttachment(root);
	downSpline->SetRelativeLocation(FVector(-125, 0, 0));
	downSpline->SetRelativeRotation(FRotator(0, -180, 0));
	

	rightSpline = CreateDefaultSubobject<USplineComponent>("Right Spline");
	rightSpline->SetupAttachment(root);
	rightSpline->SetRelativeLocation(FVector(0, 125, 0));
	rightSpline->SetRelativeRotation(FRotator(0, 90,0));

	junctionOverlap = CreateDefaultSubobject<UBoxComponent>("Junction Overlap");
	junctionOverlap->SetupAttachment(root);
	junctionOverlap->SetRelativeLocation(FVector::ZeroVector);
	junctionOverlap->SetBoxExtent(FVector(100, 100, 100));
	junctionOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	junctionOverlap->SetCollisionObjectType(ECC_GameTraceChannel1);
	junctionOverlap->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
}

// Called when the game starts or when spawned
void AJunction::BeginPlay()
{
	Super::BeginPlay();
	
	this->Tags.Add(FName(*junctionTag));
}

// Called every frame
void AJunction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJunction::GetTraversableDirections(bool &up, bool &left, bool &down, bool &right)
{
	up = hasUpSpline;
	left = hasLeftSpline;
	down = hasDownSpline;
	right = hasRightSpline;
}

void AJunction::GetEntranceDirection(bool &up, bool &left, bool &down, bool &right)
{
	up = entranceUp;
	left = entranceLeft;
	down = entranceDown;
	right = entranceRight;
}

