// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Junction.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AJunction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJunction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USceneComponent* root;

	UPROPERTY(EditAnywhere)
	class USplineComponent* upSpline;
	UPROPERTY(EditAnywhere)
	class USplineComponent* leftSpline;
	UPROPERTY(EditAnywhere)
	class USplineComponent* downSpline;
	UPROPERTY(EditAnywhere)
	class USplineComponent* rightSpline;

	UPROPERTY(EditAnywhere)
	FString junctionTag;

	UPROPERTY(EditAnywhere)
	int junctionID = 0;

	UPROPERTY(EditAnywhere)
	class AJunction* upJunction;
	UPROPERTY(EditAnywhere)
	class AJunction* leftJunction;
	UPROPERTY(EditAnywhere)
	class AJunction* downJunction;
	UPROPERTY(EditAnywhere)
	class AJunction* rightJunction;

	bool hasEntranceBeenSet = false;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* junctionOverlap;

	UPROPERTY(EditAnywhere)
	bool hasUpSpline = true;
	UPROPERTY(EditAnywhere)
	bool hasLeftSpline = true;
	UPROPERTY(EditAnywhere)
	bool hasDownSpline = true;
	UPROPERTY(EditAnywhere)
	bool hasRightSpline = true;

	UPROPERTY(EditAnywhere)
	bool entranceUp = false;
	UPROPERTY(EditAnywhere)
	bool entranceLeft = false;
	UPROPERTY(EditAnywhere)
	bool entranceDown = true;
	UPROPERTY(EditAnywhere)
	bool entranceRight = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetTraversableDirections(bool &UP, bool &left, bool &down, bool &right);
	void GetEntranceDirection(bool &UP, bool &left, bool &down, bool &right);

	FORCEINLINE class USplineComponent* GetUpSpline() { return upSpline; };
	FORCEINLINE class USplineComponent* GetLeftSpline() { return leftSpline; };
	FORCEINLINE class USplineComponent* GetDownSpline() { return downSpline; };
	FORCEINLINE class USplineComponent* GetRightSpline() { return rightSpline; };

	FORCEINLINE class AJunction* GetUpJunction() { return upJunction; };
	FORCEINLINE class AJunction* GetLeftJunction() { return leftJunction; };
	FORCEINLINE class AJunction* GetDownJunction() { return downJunction; };
	FORCEINLINE class AJunction* GetRightJunction() { return rightJunction; };

	FORCEINLINE void SetUpSpline(class USplineComponent* uSpline) { upSpline = uSpline; };
	FORCEINLINE void SetLeftSpline(class USplineComponent* lSpline) { leftSpline = lSpline; };
	FORCEINLINE void SetDownSpline(class USplineComponent* dSpline) { downSpline = dSpline; };
	FORCEINLINE void SetRightSpline(class USplineComponent* rSpline) { rightSpline = rSpline; };

	FORCEINLINE bool EntranceHasBeenSet() { return hasEntranceBeenSet; };
	FORCEINLINE void SetEntrance() { hasEntranceBeenSet = true; };

	FORCEINLINE FString GetJunctionTag() { return junctionTag; };

	FORCEINLINE int GetJunctionID() { return junctionID; };
};
