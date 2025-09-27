// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "DustFall/Core/Interface/GameInstanceInterface.h"
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
class DUSTFALL_API UDF_MainGameInstance : public UAdvancedFriendsGameInstance, public IGameInstanceInterface
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
	UFindSessionsCallbackProxyAdvanced* AdvancedFindSessions(const FString& SessionName);

	UFUNCTION(BlueprintCallable)
	void AdvancedJoinSession(const FString& SessionName, const int32& SessionIndex);

	virtual void Init() override;
	virtual void InitUniquePlayerId();

	/** Interfaces */
	virtual void SetPlayerFace_Implementation(FName NewFaceRowName) override;
	virtual void StartGame_Implementation() override;

private:
	IOnlineSessionPtr SessionInterface;
	
	UPROPERTY()
	FUniqueNetIdRepl UniquePlayerNetId;
	
	UPROPERTY()
	UCreateSessionCallbackProxyAdvanced* CreateProxySession;

	/** Delegates */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteHandle;

	/** Internal callbacks */
	UFUNCTION()
	void OnCreateSessionSuccess();

	UFUNCTION()
	void OnCreateSessionFailure();

	UFUNCTION()
	void OnDestroySessionSuccess();

	UFUNCTION()
	void OnDestroySessionFailure();

	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) override;
};
