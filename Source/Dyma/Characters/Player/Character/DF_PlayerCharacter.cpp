// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_PlayerCharacter.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Camera/CameraComponent.h"
#include "Dyma/Characters/Player/State/DF_PlayerState.h"
#include "Dyma/Core/GameInstance/DF_MainGameInstance.h"
#include "Dyma/Core/GameState/DF_GameState.h"
#include "Dyma/Core/Structures/Face.h"
#include "Dyma/UI/Interfaces/HUDInterface.h"
#include "Dyma/UI/Manager/UIManager.h"
#include "Interfaces/VoiceInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Net/VoiceConfig.h"


ADF_PlayerCharacter::ADF_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADF_PlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		CameraComponent = FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			float FielOfView = UKismetMathLibrary::Lerp(CameraComponent->FieldOfView, TargetFov, GetWorld()->GetDeltaSeconds() * 12.f);
			CameraComponent->SetFieldOfView(FielOfView);
		}
	}
}

void ADF_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraComponent = FindComponentByClass<UCameraComponent>();
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

	if (IsLocallyControlled())
	{
		if (auto GI = GetGameInstance<UDF_MainGameInstance>())
		{
			Server_SetFaceRow(GI->FaceRowName);
		}
	}
}

void ADF_PlayerCharacter::ApplyFaceByRow_Implementation(FName RowName)
{
	if (!FaceDataTable || RowName.IsNone()) return;

	const FFaceData* FaceRow = FaceDataTable->FindRow<FFaceData>(RowName, TEXT("ApplyFaceByRow"));
	if (FaceRow && CharacterMaterial)
	{
		FaceOpenTexture  = FaceRow->FaceOpen;
		FaceCloseTexture = FaceRow->FaceClose;
		CharacterMaterial->SetTextureParameterValue(FName("FaceTexture"), FaceCloseTexture);
	}
}

void ADF_PlayerCharacter::Server_SetFaceRow_Implementation(FName RowName)
{
	Multi_SetFaceRow(RowName);
}

void ADF_PlayerCharacter::Multi_SetFaceRow_Implementation(FName RowName)
{
	if (auto PS = GetPlayerState<ADF_PlayerState>())
	{
		PS->FaceRowName = RowName;
		PS->OnRep_FaceRow();
	}
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

	Server_SetMicrophoneActive(bIsNewMicrophone);
}

void ADF_PlayerCharacter::HandleInteract_Implementation(bool bIsNewInteract)
{
	if (!bIsCastingVote)
		GetWorldTimerManager().ClearTimer(VoteTimerHandle);
	
	if (bHasVoted || !HitActor) return;
	
	bIsCastingVote = bIsNewInteract;
	
	if (auto Widget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD")) 
		IHUDInterface::Execute_SetCastVote(Widget, bIsNewInteract);

	if (bIsNewInteract) {
		GetWorldTimerManager().SetTimer(
			VoteCastTimerHandle,
			this,
			&ADF_PlayerCharacter::OnVoteCast,
			2.0f,
			false);
	}
	else
		GetWorldTimerManager().ClearTimer(VoteCastTimerHandle);
}

void ADF_PlayerCharacter::HandleZoom_Implementation(bool bIsNewZoom)
{
	TargetFov = bIsNewZoom ? 40.f : 100.f;
}

void ADF_PlayerCharacter::OnVoteCast()
{
	bHasVoted = true;
	
	if (auto Widget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD")) {
		IHUDInterface::Execute_SetCastVote(Widget, false);
		IHUDInterface::Execute_SetHelpVoteTextVisible(Widget);
	}

	if (auto PS = PlayerController->GetPlayerState<ADF_PlayerState>())
		if (auto TargetPlayerState = Cast<ACharacter>(HitActor))
			PS->Server_SetVote(TargetPlayerState->GetPlayerState());
}

void ADF_PlayerCharacter::StopVoteRound_Implementation()
{
	GetWorldTimerManager().ClearTimer(VoteTimerHandle);
	
	Multi_StopVote();
}

void ADF_PlayerCharacter::NotifyPauseVoteAvailable_Implementation(int32 CountPlayers)
{
	if (!UIManager) return;

	if (auto HUD = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD"))
	{
		IHUDInterface::Execute_UpdateStartPauseVote(HUD, true, CountPlayers);
	}
}

void ADF_PlayerCharacter::Multi_StopVote_Implementation()
{
	bHasVoted = false;
	bIsCastingVote = false;
	HitActor = nullptr;
}

void ADF_PlayerCharacter::StartVoteRound_Implementation()
{
	if (!IPlayerStateInterface::Execute_GetIsParticipant(GetPlayerState())) return;
	
	GetWorld()->GetTimerManager().SetTimer(
		VoteTimerHandle,
		this,
		&ADF_PlayerCharacter::OnVotingTimer,
		0.2f,
		true
	);
}

void ADF_PlayerCharacter::StartFinalVoteRound_Implementation()
{
	if (IPlayerStateInterface::Execute_GetIsParticipant(GetPlayerState())) return;
	
	GetWorld()->GetTimerManager().SetTimer(
		VoteTimerHandle,
		this,
		&ADF_PlayerCharacter::OnVotingFinalTimer,
		0.2f,
		true
	);
}

void ADF_PlayerCharacter::KickedPlayerName_Implementation(const FString& PlayerName)
{
	if (!UIManager) return;

	if (PlayerController->IsLocalPlayerController())
		if (auto HUDWidget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD"))
			IHUDInterface::Execute_SetKickedPlayerName(HUDWidget, PlayerName);
}

void ADF_PlayerCharacter::OnVotingTimer_Implementation()
{
	if (!PlayerController)
		PlayerController = Cast<APlayerController>(GetController());
	
	if (PlayerController && CameraComponent)
	{
		int32 ViewportX, ViewportY;
		FVector StartDirection, EndDirection;
	
		PlayerController->GetViewportSize(ViewportX, ViewportY);
		PlayerController->DeprojectScreenPositionToWorld(ViewportX / 2, ViewportY / 2, StartDirection, EndDirection);

		FRotator CameraRotation = CameraComponent->GetComponentRotation();
		FVector End = StartDirection + (CameraRotation.Vector() * 700.0f);
		
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartDirection, End, ECC_Visibility, CollisionParams);
		if (auto NewHitActor = HitResult.GetActor(); bHit && NewHitActor)
		{
			if (!NewHitActor || bHasVoted || bIsCastingVote || NewHitActor == HitActor) return;
			
			ACharacter* NewCharacter = Cast<ACharacter>(NewHitActor);
			if (!NewCharacter) return;
			
			if (!IPlayerStateInterface::Execute_GetIsParticipant(NewCharacter->GetPlayerState())) return;
			
			auto HUDWidget = IPlayerToUIInterface::Execute_GetUI(UIManager, "HUD");
			if (!HUDWidget) return;
			
			HitActor = NewHitActor;

			if (PlayerController->IsLocalPlayerController())
				IHUDInterface::Execute_SetVoteText(HUDWidget, NewCharacter->GetPlayerState()->GetPlayerName().Left(10));
		}
	}
}

void ADF_PlayerCharacter::OnVotingFinalTimer_Implementation()
{
	if (!PlayerController) PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && CameraComponent)
	{
		int32 ViewportX, ViewportY;
		FVector StartDirection, EndDirection;
		
		PlayerController->GetViewportSize(ViewportX, ViewportY);
		PlayerController->DeprojectScreenPositionToWorld(ViewportX / 2, ViewportY / 2, StartDirection, EndDirection);
		
		FRotator CameraRotation = CameraComponent->GetComponentRotation();
		FVector End = StartDirection + (CameraRotation.Vector() * 1000.0f);
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		
		CollisionParams.AddIgnoredActor(this);
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartDirection, End, ECC_Visibility, CollisionParams);
		if (auto NewHitActor = HitResult.GetActor(); bHit && NewHitActor)
		{
			if (!NewHitActor || bHasVoted || bIsCastingVote || NewHitActor == HitActor) return;
			
			ACharacter* NewCharacter = Cast<ACharacter>(NewHitActor);
			if (!NewCharacter) return;
			
			if (!IPlayerStateInterface::Execute_GetIsParticipant(NewCharacter->GetPlayerState())) return;
			
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

void ADF_PlayerCharacter::Server_SetMicrophoneActive_Implementation(bool bIsActive)
{
	Multi_SetMicrophoneActive(bIsActive);
}

void ADF_PlayerCharacter::Multi_SetMicrophoneActive_Implementation(bool bIsActive)
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

	DOREPLIFETIME(ADF_PlayerCharacter, HitActor);
	DOREPLIFETIME(ADF_PlayerCharacter, bHasVoted);
	DOREPLIFETIME(ADF_PlayerCharacter, bIsCastingVote);
	DOREPLIFETIME(ADF_PlayerCharacter, FaceOpenTexture);
	DOREPLIFETIME(ADF_PlayerCharacter, FaceCloseTexture);
}