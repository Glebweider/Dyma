// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxGame.generated.h"

UCLASS()
class DYMA_API ABoxGame : public AActor
{
	GENERATED_BODY()

public:
	ABoxGame();

protected:
	virtual void BeginPlay() override;
	
};
