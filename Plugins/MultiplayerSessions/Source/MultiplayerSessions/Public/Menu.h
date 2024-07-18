// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "MultiplayerSessions")
	void	MenuSetup(int32 InNumPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:
	//OnLevelRemovedFromWorld()
	virtual void NativeDestruct() override;

	/*
	 * CallBacks
	 */
	UFUNCTION()
	void	OnCreateSession(bool bWasSuccessful);
	void	OnFindSessions(const TArray<FOnlineSessionSearchResult> &SessionSearchResults, bool bWasSuccessful);
	void	OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void	OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void	OnStartSession(bool bWasSuccessful);
	

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

	UFUNCTION()
	void	HostButtonClicked();
	UFUNCTION()
	void	JoinButtonClicked();

	void	MenuTearDown();

	//handle online session functionallity
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	int32	NumPublicConnections {4};
	FString	MatchType {TEXT("FreeForAll")};
	FString	PathToLobby {TEXT("")};
	
public:
	virtual bool Initialize() override;

	void ScreenMessage(FString Message);
};
