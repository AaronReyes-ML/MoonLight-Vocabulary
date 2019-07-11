// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPickup.generated.h"

UCLASS()
class TRAMPOLINEGAME_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* overlap;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* mesh;
	UPROPERTY(EditAnywhere)
	class UStaticMesh* notTakenMesh;
	UPROPERTY(EditAnywhere)
	class UStaticMesh* takenMesh;

	UPROPERTY(EditAnywhere)
	bool hasBeenPickedUp = false;
	UPROPERTY(EditAnywhere)
	int autoFeverCount = 0;
	UPROPERTY(EditAnywhere)
	int questionReduceCount = 0;
	UPROPERTY(EditAnywhere)
	int supportCount = 0;
	UPROPERTY(EditAnywhere)
	int itemPickupID = 0;

	UParticleSystem* pickupParticleSystem;
	class UParticleSystemComponent* pickupParticleSystemComponent;

	UPROPERTY(EditAnywhere)
	class UPointLightComponent* itemPickupLight;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE bool GetHasBeenPickedUp() { return hasBeenPickedUp; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetAutoFeverCount() { return autoFeverCount; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetQuestionReduceCount() { return questionReduceCount; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetSupportCount() { return supportCount; };
	void SetHasPickedUp(bool pickedup);
	FORCEINLINE int GetItemPickupID() { return itemPickupID; };

	void DoPlayPickupParticles();
	void EnableLight();
};
