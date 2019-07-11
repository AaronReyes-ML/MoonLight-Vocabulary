// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CinematicPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoPlayDelegate);

UCLASS()
class TRAMPOLINEGAME_API ACinematicPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACinematicPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TArray<FString> cutsceneTextArray;

	UPROPERTY(EditAnywhere)
		int totalArrayMembers = 0;

	UPROPERTY(EditAnywhere)
		int currentTextIndex;

	UFUNCTION(BlueprintCallable)
		void ChangeTextIndex(bool forward);

	UFUNCTION(BlueprintCallable)
		void EnableProgress();

	UFUNCTION(BlueprintCallable)
		void DisableProgress();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UIWidgetBase;

	UPROPERTY(EditAnywhere)
	bool end = false;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetStringAtIndex(int index) { return cutsceneTextArray[index]; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetCurrentIndex() { return currentTextIndex; };

	UPROPERTY(EditAnywhere)
	class UTrampolineCinematicWidget* UIWidget;

	void DoPlay();

	UPROPERTY(BlueprintAssignable, Category = "Test")
	FDoPlayDelegate OnDoPlayDelegate;
};
