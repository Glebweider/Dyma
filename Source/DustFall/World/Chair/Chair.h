// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chair.generated.h"

class ANameplate;

UCLASS()
class DUSTFALL_API AChair : public AActor
{
	GENERATED_BODY()

public:
	AChair();

	UFUNCTION(Reliable, Server)
	void Server_UpdateNameplate(const FString& PlayerName);

	UPROPERTY(EditAnywhere)
	ANameplate* Nameplate;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Reliable, NetMulticast)
	virtual void Multicast_UpdateNameplate(const FString& PlayerName);

	UFUNCTION(Reliable, Client)
	virtual void Client_UpdateNameplate(const FString& PlayerName);

	UFUNCTION()
	void UpdateNameplate(const FString& PlayerName);

};
