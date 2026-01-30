// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DYMA_API IInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StartInteract(ACharacter* InteractPlayer);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StopInteract(ACharacter* InteractPlayer);
};
