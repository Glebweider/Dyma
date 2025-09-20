// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chair.generated.h"

class ABook;
class ANameplate;

UCLASS()
class DUSTFALL_API AChair : public AActor
{
	GENERATED_BODY()

public:
	AChair();

	UFUNCTION(Reliable, Server)
	void StartGame();
	
	UFUNCTION(Reliable, NetMulticast)
	void Multi_StartGame();

	ACharacter* GetCharacter() const { return Character; }
	void SetCharacter(ACharacter* NewCharacter);

	UPROPERTY(EditAnywhere, Replicated)
	ANameplate* Nameplate;

	UPROPERTY(EditAnywhere, Replicated)
	ABook* Book;

protected:
	virtual void Destroyed() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Character();

	UFUNCTION(Reliable, Server)
	void Server_UpdateNameplate(ACharacter* NewCharacter);

	UFUNCTION(Reliable, NetMulticast)
	virtual void UpdateNameplate(ACharacter* NewCharacter);

	UPROPERTY(ReplicatedUsing=OnRep_Character)
	ACharacter* Character;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetLeftClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetRightClass;
};
