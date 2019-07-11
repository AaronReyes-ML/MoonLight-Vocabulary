// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WolfAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TRAMPOLINEGAME_API UWolfAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool isRunning = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool isJumping = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool isCharging = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool isFeverReady = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool isTransendanceActive = false;
};
