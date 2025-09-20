// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_PlayerCharacter.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Camera/CameraComponent.h"
#include "DustFall/Characters/Player/State/DF_PlayerState.h"
#include "DustFall/Core/GameInstance/DF_MainGameInstance.h"
#include "DustFall/Core/Structures/Face.h"
#include "DustFall/UI/Interfaces/HUDInterface.h"
#include "DustFall/UI/Manager/UIManager.h"
#include "Interfaces/VoiceInterface.h"
#include "Net/UnrealNetwork.h"
#include "Net/VoiceConfig.h"


ADF_PlayerCharacter::ADF_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADF_PlayerCharacter::HandleCrouch_Implementation(bool bIsNewCrouch)
{
	if (bIsNewCrouch)
		Crouch();
	else
		UnCrouch();
}

void ADF_PlayerCharacter::HandleMicrophone_Implementation(bool bIsNewMicrophone)
{
	if (IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld()))
		if (auto Voice = OSS->GetVoiceInterface())
			if (bIsNewMicrophone)
			{
				Voice->RegisterLocalTalker(0);
				Voice->StartNetworkedVoice(0);				
			} else
			{
				Voice->StopNetworkedVoice(0);
				Voice->UnregisterLocalTalker(0);
			}

	ServerSetMicrophoneActive(bIsNewMicrophone);
}

void ADF_PlayerCharacter::HandleInteract_Implementation(bool bIsNewInteract)
{
	if (!bIsCastingVote)
		GetWorldTimerManager().ClearTimer(VoteTimerHandle);
	
	if (bHasVoted || !HitActor) return;
	
	bIsCastingVote = bIsNewInteract;
	
	if (auto Widget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD")) 
		IHUDInterface::Execute_SetCastVote(Widget, bIsNewInteract);

	if (bIsNewInteract)
		GetWorldTimerManager().SetTimer(
			VoteCastTimerHandle,
			this,
			&ADF_PlayerCharacter::OnVoteCast,
			2.0f,
			false);
	else
		GetWorldTimerManager().ClearTimer(VoteCastTimerHandle);
}

void ADF_PlayerCharacter::OnVoteCast()
{
	bHasVoted = true;
	
	if (auto Widget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD")) {
		IHUDInterface::Execute_SetCastVote(Widget, false);
		IHUDInterface::Execute_SetHelpVoteTextVisible(Widget);
	}

	GetWorldTimerManager().ClearTimer(VoteTimerHandle);

	if (auto PS = PlayerController->GetPlayerState<ADF_PlayerState>())
		if (auto TargetPlayerState = Cast<ACharacter>(HitActor))
			PS->Server_SetVote(TargetPlayerState->GetPlayerState());
}

void ADF_PlayerCharacter::Multi_StartVoteRound_Implementation()
{
	GetWorldTimerManager().SetTimer(
		VoteTimerHandle,
		this,
		&ADF_PlayerCharacter::OnVotingTimer,
		0.3f,
		true
	);
}

void ADF_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UIManager = PlayerController->FindComponentByClass<UUIManager>();
		
		if (UIManager && LobbyWidget)
			IPlayerToUIInterface::Execute_ShowUI(UIManager, LobbyWidget);

		RegisterRemoteTalker(PlayerController->PlayerState);
	}

	if (UStaticMeshComponent* StMesh = GetComponentByClass<UStaticMeshComponent>())
		if (UMaterialInterface* BaseMaterial = StMesh->GetMaterial(0))
		{
			CharacterMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			StMesh->SetMaterial(0, CharacterMaterial);
		}

	if (auto GI = GetGameInstance<UDF_MainGameInstance>())
	{
		const FName RowName = GI->FaceRowName;
		
		if (FaceDataTable)
		{
			const FFaceData* FaceRow = FaceDataTable->FindRow<FFaceData>(RowName, TEXT("CharacterBeginPlay"));
			if (FaceRow)
			{
				FaceOpenTexture  = FaceRow->FaceOpen;
				FaceCloseTexture = FaceRow->FaceClose;

				UE_LOG(LogTemp, Log, TEXT("Loaded FaceOpen: %s  FaceClose: %s  FaceRowName: %s"),
					*GetNameSafe(FaceOpenTexture),
					*GetNameSafe(FaceCloseTexture),
					*RowName.ToString());
				
				if (CharacterMaterial)
					CharacterMaterial->SetTextureParameterValue(FName("FaceTexture"), FaceCloseTexture);
			}
		}
	}
}

void ADF_PlayerCharacter::StartVoteRound_Implementation()
{
	Multi_StartVoteRound();
}

void ADF_PlayerCharacter::KickedPlayerName_Implementation(const FString& PlayerName)
{
	if (!UIManager) return;

	if (PlayerController->IsLocalPlayerController())
		if (auto HUDWidget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD"))
			IHUDInterface::Execute_SetKickedPlayerName(HUDWidget, PlayerName);
}

void ADF_PlayerCharacter::OnVotingTimer()
{
	UCameraComponent* CameraComponent = FindComponentByClass<UCameraComponent>();
	if (PlayerController && CameraComponent)
	{
		int32 ViewportX, ViewportY;
		FVector StartDirection, EndDirection;
	
		PlayerController->GetViewportSize(ViewportX, ViewportY);
		PlayerController->DeprojectScreenPositionToWorld(ViewportX / 2, ViewportY / 2, StartDirection, EndDirection);

		FRotator CameraRotation = CameraComponent->GetComponentRotation();
		FVector End = StartDirection + (CameraRotation.Vector() * 520.0f);
		
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartDirection, End, ECC_Visibility, CollisionParams);
		if (auto NewHitActor =  HitResult.GetActor(); bHit && NewHitActor)
		{
			if (bHasVoted || bIsCastingVote || NewHitActor == HitActor) return;

			ACharacter* NewCharacter = Cast<ACharacter>(NewHitActor);
			if (!NewCharacter) return;

			auto HUDWidget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD");
			if (!HUDWidget) return;

			HitActor = NewHitActor;

			if (PlayerController->IsLocalPlayerController())
				IHUDInterface::Execute_SetVoteText(HUDWidget, NewCharacter->GetPlayerState()->GetPlayerName().Left(10));
		}
	}
}

void ADF_PlayerCharacter::RegisterRemoteTalker(APlayerState* RemotePlayerState)
{
	if (!RemotePlayerState) return;

	if (IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld()))
		if (auto Voice = OSS->GetVoiceInterface())
		{
			TSharedPtr<const FUniqueNetId> RemoteId = RemotePlayerState->GetUniqueId().GetUniqueNetId();
			if (RemoteId.IsValid())
				Voice->RegisterRemoteTalker(*RemoteId);
		}
}

void ADF_PlayerCharacter::ServerSetMicrophoneActive_Implementation(bool bIsActive)
{
	MulticastSetMicrophoneActive(bIsActive);
}

void ADF_PlayerCharacter::MulticastSetMicrophoneActive_Implementation(bool bIsActive)
{
	if (CharacterMaterial)
		CharacterMaterial->SetTextureParameterValue(FName("FaceTexture"), bIsActive ? FaceOpenTexture : FaceCloseTexture);

	if (UIManager)
		if (auto Widget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD"))
			IHUDInterface::Execute_UpdateMicrophoneState(Widget, bIsActive);
}

void ADF_PlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADF_PlayerCharacter, FaceOpenTexture);
	DOREPLIFETIME(ADF_PlayerCharacter, FaceCloseTexture);
}