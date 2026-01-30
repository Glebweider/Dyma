// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dyma/Core/Interface/InteractInterface.h"
#include "Dyma/Core/Interface/WorldItemInterface.h"
#include "GameFramework/Actor.h"
#include "ChairBar.generated.h"

UCLASS()
class DYMA_API AChairBar : public AActor, public IInteractInterface, public IWorldItemInterface
{
	GENERATED_BODY()

public:
	AChairBar();

	/** Interfaces */
	virtual void StartInteract_Implementation(ACharacter* InteractPlayer) override;
	virtual void LeaveSeat_Implementation(ACharacter* Player) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Reliable)
	void ServerStartInteract(ACharacter* InteractPlayer);

	UFUNCTION(Server, Reliable)
	void ServerLeaveSeat(ACharacter* Player);
	
	void HandleStartInteract(ACharacter* InteractPlayer);
	void HandleLeaveSeat(ACharacter* Player);
	
	UPROPERTY(Replicated)
	ACharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SoundBusy;
};
