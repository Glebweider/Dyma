// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameInstanceInterface.generated.h"

class UBaseUserWidget;
// This class does not need to be modified.
UINTERFACE()
class UGameInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUSTFALL_API IGameInstanceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetPlayerFace(FName NewFaceRowName);
};
