// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chair.generated.h"

class ABook;
class ANameplate;

UCLASS()
class DYMA_API AChair : public AActor
{
	GENERATED_BODY()

public:
	AChair();

	/** Server RPC's */
	UFUNCTION(Reliable, Server)
	void Server_StartGame();

	/** Multicast RPC's */
	UFUNCTION(Reliable, NetMulticast)
	void Multi_StartGame();

	UPROPERTY(Replicated)
	ACharacter* Character;

	UPROPERTY(EditAnywhere, Replicated)
	ANameplate* Nameplate;

	UPROPERTY(EditAnywhere, Replicated)
	ABook* Book;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetLeftClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetRightClass;
};
