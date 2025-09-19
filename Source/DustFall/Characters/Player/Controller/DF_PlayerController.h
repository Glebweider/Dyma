// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "DF_PlayerController.generated.h"

struct FProjectData;
class UBaseUserWidget;
/**
 * 
 */
UCLASS()
class DUSTFALL_API ADF_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void ClientStartGame(const FProjectData& InProject);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	/** Actions */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MicrophoneAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CrouchAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PauseMenuAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractAction;

	/** Variables */
	UPROPERTY(Replicated)
	ACharacter* ControlledCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UActorComponent* UIManager;

	/** Input handlers */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartInteract();
	void StopInteract();
	void StartJump();
	void StopJump();
	void StartMicrophone();
	void StopMicrophone();
	void StartCrouch();
	void StopCrouch();
	void PauseMenu();
};
