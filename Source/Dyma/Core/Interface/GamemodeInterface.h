// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GamemodeInterface.generated.h"

class UBaseUserWidget;
// This class does not need to be modified.
UINTERFACE()
class UGamemodeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DYMA_API IGamemodeInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void AnvilOverlapPlayer();
};
