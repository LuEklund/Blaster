// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * Menu callback delegates
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionCompleteDelegate, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionCompleteDelegate, const TArray<FOnlineSessionSearchResult> &SessionSearchResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionCompleteDelegate, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionCompleteDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionCompleteDelegate, bool, bWasSuccessful);

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	//handle session functionallity
	void	CreateSession(int32 NumPublicConnections, FString MatchType);
	void	FindSession(int32 MaxSearchResults);
	void	JoinSession(const FOnlineSessionSearchResult &SessionResult);
	void	DestroySession();
	void	StartSession();

	/*
	 * 
	 */
	FMultiplayerOnCreateSessionCompleteDelegate	    MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionCompleteDelegate	    MultiplayerOnFindSessionComplete;
	FMultiplayerOnJoinSessionCompleteDelegate	    MultiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionCompleteDelegate	MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionCompleteDelegate		MultiplayerOnStartSessionComplete;

protected:

	/*
	 * Interal callbacks for the delegates we will add to the online session interface delegates list
	 * these dont need to be called outside this class.
	 */
	void	OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void	OnFindSessionsComplete(bool bWasSuccessful);
	void	OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void	OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void	OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr					SessionInterface;
	TSharedPtr<FOnlineSessionSettings>	LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch>	LastSessionSearch;

	/*
	 * To add to the online session interface delegate list.
	 * We will bind our MultiplayerSessionSubsystem Internal callbacks to these,
	 */

	FOnCreateSessionCompleteDelegate	CreateSessionCompleteDelegate;
	FDelegateHandle						CreateSessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate		FindSessionsCompleteDelegate;
	FDelegateHandle						FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate		JoinSessionCompleteDelegate;
	FDelegateHandle						JoinSessionCompleteDelegateHandle;
	
	FOnDestroySessionCompleteDelegate	DestroySessionCompleteDelegate;
	FDelegateHandle						DestroySessionCompleteDelegateHandle;
	
	FOnStartSessionCompleteDelegate		StartSessionCompleteDelegate;
	FDelegateHandle						StartSessionCompleteDelegateHandle;

	bool	bCreateSessionOnDestroy{false};
	int32	LastNumPublicConnections;
	FString	LastMatchType;

};
