// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Dyma/Characters/Player/Interfaces/InputToPlayerInterface.h"
#include "Dyma/Characters/Player/State/DF_PlayerState.h"
#include "Dyma/Core/Interface/WorldItemInterface.h"
#include "Dyma/Core/UserSettings/DF_UserSettings.h"
#include "Dyma/UI/Interfaces/HUDInterface.h"
#include "Dyma/UI/Manager/UIManager.h"
#include "Dyma/UI/Widgets/Lobby/LobbyWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

void ADF_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UIManager = FindComponentByClass<UUIManager>();
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			if (InputMappingContext)
				Subsystem->AddMappingContext(InputMappingContext, 0);
	
	UserSettings = Cast<UDF_UserSettings>(GEngine->GetGameUserSettings());
}

void ADF_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledCharacter = Cast<ACharacter>(InPawn);

	if (ControlledCharacter)
		if (auto Movement = ControlledCharacter->GetCharacterMovement())
			Movement->DisableMovement();
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
		if (MicrophoneAction)
		{
			EnhancedInput->BindAction(MicrophoneAction, ETriggerEvent::Started, this, &ADF_PlayerController::StartMicrophone);
			EnhancedInput->BindAction(MicrophoneAction, ETriggerEvent::Completed, this, &ADF_PlayerController::StopMicrophone);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADF_PlayerController::Look);
		}
		if (PauseMenuAction)
		{
			EnhancedInput->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &ADF_PlayerController::PauseMenu);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &ADF_PlayerController::StartInteract);
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Completed, this, &ADF_PlayerController::StopInteract);
		}
		if (ZoomAction)
		{
			EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Started, this, &ADF_PlayerController::StartZoom);
			EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Completed, this, &ADF_PlayerController::StopZoom);
		}
	}
}

void ADF_PlayerController::Client_StartGame_Implementation(const FProjectData& InProject, const FString& InEvent)
{
	SetIgnoreLookInput(false);

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
	
	if (auto PS = Cast<ADF_PlayerState>(PlayerState))
	{
		PS->SetProject(InProject);
		PS->SetEvent(InEvent);
	}
	
	if (UIManager)
		if (auto WidgetLobby = IPlayerToUIInterface::Execute_GetUI(UIManager, "Lobby Menu"))
			ILobbyInterface::Execute_StartGame(WidgetLobby);
		else if (auto WidgetHUD = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD"))
			IHUDInterface::Execute_StartGame(WidgetHUD);
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

	float MouseSensitivity = UserSettings->GetMouseSensitivity();
	
	AddYawInput(LookAxis.X * MouseSensitivity);
	AddPitchInput(LookAxis.Y * MouseSensitivity);
}

void ADF_PlayerController::StartInteract()
{
	if (ControlledCharacter)
		IInputToPlayerInterface::Execute_HandleInteract(ControlledCharacter, true);
}

void ADF_PlayerController::StopInteract()
{
	if (ControlledCharacter)
		IInputToPlayerInterface::Execute_HandleInteract(ControlledCharacter, false);
}

void ADF_PlayerController::StartJump()
{
	if (ControlledCharacter)
		ControlledCharacter->Jump();
	
	if (AActor* SeatActor = IPlayerStateInterface::Execute_GetActorSeat(PlayerState))
		IWorldItemInterface::Execute_LeaveSeat(SeatActor, ControlledCharacter);
}

void ADF_PlayerController::StopJump()
{
	if (ControlledCharacter)
		ControlledCharacter->StopJumping();
}

void ADF_PlayerController::StartMicrophone()
{
	if (ControlledCharacter)
		IInputToPlayerInterface::Execute_HandleMicrophone(ControlledCharacter, true);
}

void ADF_PlayerController::StopMicrophone()
{
	if (ControlledCharacter)
		IInputToPlayerInterface::Execute_HandleMicrophone(ControlledCharacter, false);
}

void ADF_PlayerController::PauseMenu()
{
	if (UIManager)
		IPlayerToUIInterface::Execute_HandleEscape(UIManager);
}

void ADF_PlayerController::StartZoom()
{
	if (ControlledCharacter)
		IInputToPlayerInterface::Execute_HandleZoom(ControlledCharacter, true);
}

void ADF_PlayerController::StopZoom()
{
	if (ControlledCharacter)
		IInputToPlayerInterface::Execute_HandleZoom(ControlledCharacter, false);
}

void ADF_PlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADF_PlayerController, ControlledCharacter);
}
