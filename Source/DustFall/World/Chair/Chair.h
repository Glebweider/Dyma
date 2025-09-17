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

	UFUNCTION(BlueprintCallable)
	ACharacter* GetCharacter();
	
	void SetCharacter(ACharacter* NewCharacter);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Character();

	UPROPERTY(ReplicatedUsing=OnRep_Character)
	ACharacter* Character;
};
