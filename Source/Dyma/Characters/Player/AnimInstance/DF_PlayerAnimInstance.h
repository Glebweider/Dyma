// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Dyma/Characters/Player/Interfaces/PlayerAnimInstanceInterface.h"
#include "DF_PlayerAnimInstance.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class DYMA_API UDF_PlayerAnimInstance : public UAnimInstance, public IPlayerAnimInstanceInterface
{
	GENERATED_BODY()

public:
	virtual void SetIsSitting_Implementation(bool bIsNewSitting) override { bIsSitting = bIsNewSitting; };

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Direction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsFalling;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsSitting = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Velocity;

private:
	UPROPERTY()
	ACharacter* Character;

	UPROPERTY()
	UCharacterMovementComponent* MovementComponent;
};
