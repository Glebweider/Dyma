// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DustFall/Characters/Player/Interfaces/InputToPlayer/InputToPlayerInterface.h"
#include "DustFall/UI/Manager/UIManager.h"
#include "DustFall/UI/Widgets/LobbyWidget/LobbyWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void ADF_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	ControlledCharacter = Cast<ACharacter>(GetPawn());
	
	UIManager = FindComponentByClass<UUIManager>();
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void ADF_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADF_PlayerController::Move);
		}
		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ADF_PlayerController::StartJump);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADF_PlayerController::StopJump);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADF_PlayerController::Look);
		}
		if (CrouchAction)
		{
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ADF_PlayerController::StartCrouch);
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ADF_PlayerController::StopCrouch);
		}
		if (PauseMenuAction)
		{
			EnhancedInput->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &ADF_PlayerController::PauseMenu);
		}
	}
}

void ADF_PlayerController::ClientStartGame_Implementation()
{
	if (UIManager)
	{
		IPlayerToUIInterface::Execute_ChangeVisibilityWidgetByName(UIManager, "Lobby Menu");
		
		if (ACharacter* MyPawn = Cast<ACharacter>(GetPawn())) {
			SetViewTarget(MyPawn);
			MyPawn->GetCharacterMovement()->DisableMovement();
		}
	}
}

void ADF_PlayerController::Move(const FInputActionValue& Value)
{
	if (ControlledCharacter)
	{
		FVector2D MovementVector = Value.Get<FVector2D>();
		FRotator YawRotation(0, GetControlRotation().Yaw, 0);
		
		FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledCharacter->AddMovementInput(ForwardDir, MovementVector.Y);
		ControlledCharacter->AddMovementInput(RightDir, MovementVector.X);		
	}
}

void ADF_PlayerController::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();
	
	AddYawInput(LookAxis.X);
	AddPitchInput(LookAxis.Y);		
}

void ADF_PlayerController::StartJump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Jump();
	}
}

void ADF_PlayerController::StopJump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->StopJumping();
	}
}

void ADF_PlayerController::StartCrouch()
{
	if (ControlledCharacter)
	{
		IInputToPlayerInterface::Execute_HandleCrouch(ControlledCharacter, true);
	}
}

void ADF_PlayerController::StopCrouch()
{
	if (ControlledCharacter)
	{
		IInputToPlayerInterface::Execute_HandleCrouch(ControlledCharacter, false);
	}
}

void ADF_PlayerController::PauseMenu()
{
	if (UIManager)
	{
		IPlayerToUIInterface::Execute_HandleEscape(UIManager);
	}
}
