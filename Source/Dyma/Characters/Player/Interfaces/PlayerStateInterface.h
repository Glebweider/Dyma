// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Dyma/Core/Structures/Project.h"
#include "PlayerStateInterface.generated.h"

UINTERFACE()
class UPlayerStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DYMA_API IPlayerStateInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	FProjectData GetProjectData();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	FString GetProjectEvent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	float GetAlkoDuration();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetAlkoDuration(float NewDuration);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	AActor* GetActorSeat();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetActorSeat(AActor* NewActor);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	bool GetIsParticipant();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetIsParticipant(bool bIsNewParticipant);
};
