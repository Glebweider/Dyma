// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Dyma/Core/Enums/SittingPoses.h"
#include "PlayerAnimInstanceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPlayerAnimInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DYMA_API IPlayerAnimInstanceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetIsSitting(bool bIsNewSitting);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetSittingPoses(ESittingPoses NewPoses);
};
