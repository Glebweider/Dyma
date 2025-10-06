// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FaceSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DYMA_API UFaceSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Face")
	FName SavedFaceRowName;
};
