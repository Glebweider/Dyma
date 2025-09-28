// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bench.generated.h"

UCLASS()
class DUSTFALL_API ABench : public AActor
{
	GENERATED_BODY()

public:
	ABench();

	TArray<ACharacter*> GetOccupants() { return Occupants; }
	void LeaveSeat(ACharacter* Player);

	UFUNCTION(BlueprintCallable)
	bool SeatPlayer(ACharacter* Player);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<USceneComponent*> SeatPoints;
	
	UPROPERTY()
	TArray<ACharacter*> Occupants;
};
