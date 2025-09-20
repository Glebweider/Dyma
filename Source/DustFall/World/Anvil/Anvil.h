// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Anvil.generated.h"

UCLASS()
class DUSTFALL_API AAnvil : public AActor
{
	GENERATED_BODY()

public:
	AAnvil();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	bool bHasCrushedPlayer = false;
};
