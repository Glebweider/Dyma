// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Nameplate.generated.h"

class UTextRenderComponent;

UCLASS()
class DUSTFALL_API ANameplate : public AActor
{
	GENERATED_BODY()

public:
	ANameplate();

	virtual void RenderText(const FString& Nickname);

	UPROPERTY(ReplicatedUsing=OnRep_Username)
	FString Username;

	UFUNCTION()
	void OnRep_Username();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
