// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_PlayerAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"

void UDF_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(TryGetPawnOwner());
	
	if (Character)
		MovementComponent = Character->GetCharacterMovement();
}

void UDF_PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character || !MovementComponent || !Character->GetPlayerState()) return;
	
	const FVector CharVelocity = Character->GetVelocity();
	const FRotator CharRotation = Character->GetActorRotation();

	Velocity = CharVelocity;
	Speed = CharVelocity.Length();
	Direction = UKismetAnimationLibrary::CalculateDirection(CharVelocity, CharRotation);
	bIsFalling = MovementComponent->IsFalling();
}
