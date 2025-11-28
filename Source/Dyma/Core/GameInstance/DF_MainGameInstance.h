// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "Dyma/Core/Interface/GameInstanceInterface.h"
#include "Engine/GameInstance.h"
#include "DF_MainGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFaceRowNameChangedSignature, FName, NewFaceRowName);

class UFindSessionsCallbackProxyAdvanced;
class UCreateSessionCallbackProxyAdvanced;
class IOnlineSubsystem;
/**
 * 
 */
UCLASS()
class DYMA_API UDF_MainGameInstance : public UAdvancedFriendsGameInstance, public IGameInstanceInterface
{
	GENERATED_BODY()
	
public:
	FName FaceRowName;
	
	FFaceRowNameChangedSignature OnFaceRowNameChanged;
	TArray<FOnlineSessionSearchResult> OnlineSessionResults;
	
	UFUNCTION(BlueprintCallable)
	void AdvancedCreateSession(const FString& SessionName);

	UFUNCTION(BlueprintCallable)
	void AdvancedDestroySession();

	UFUNCTION(BlueprintCallable)
	void AdvancedJoinSession(const FString& SessionName, const int32& SessionIndex);

	// === Sounds ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundMix* MainSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* MasterVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* VoiceVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* SFXVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* AmbientVolumeClass;

	virtual void Init() override;
	virtual void InitUniquePlayerId();

	/** Interfaces */
	virtual void SetPlayerFace_Implementation(FName NewFaceRowName) override;
	virtual void SetSessionJoinAllowed_Implementation(bool bAllowJoin) override;

private:
	IOnlineSessionPtr SessionInterface;
	
	UPROPERTY()
	FUniqueNetIdRepl UniquePlayerNetId;
	
	UPROPERTY()
	UCreateSessionCallbackProxyAdvanced* CreateProxySession;

	/** Delegates */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteHandle;
	FDelegateHandle DestroySessionDelegateHandle;

	/** Internal callbacks */
	UFUNCTION()
	void OnCreateSessionSuccess();

	UFUNCTION()
	void OnCreateSessionFailure();

	UFUNCTION()
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) override;
};
