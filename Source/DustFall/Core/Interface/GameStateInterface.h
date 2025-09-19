// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DustFall/Core/Structures/Project.h"
#include "GameStateInterface.generated.h"

class UBaseUserWidget;
// This class does not need to be modified.
UINTERFACE()
class UGameStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUSTFALL_API IGameStateInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	FProjectData GetProjectData();
};
