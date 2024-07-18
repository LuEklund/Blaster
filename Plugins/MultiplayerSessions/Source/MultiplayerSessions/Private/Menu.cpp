// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"

void UMenu::MenuSetup(int32 InNumPublicConnections, FString TypeOfMatch,  FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = InNumPublicConnections;
	MatchType = TypeOfMatch;
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	// bIsFocusable = true;

	if (GetWorld())
	{
		if (APlayerController *PlayerController = GetWorld()->GetFirstPlayerController())
		{
			FInputModeUIOnly	InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &UMenu::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
	}
	
}



//OnLevelRemovedFromWorld()
void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Green,
			FString(TEXT("Session created successfully")));
		}
		if (GetWorld())
		{
			GetWorld()->ServerTravel(PathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString(TEXT("Failed to create Session!")));
		}
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionSearchResults, bool bWasSuccessful)
{
	ScreenMessage(FString(TEXT(" UMenu::OnFindSessions")));
	if (MultiplayerSessionsSubsystem == nullptr) return;
	ScreenMessage(FString(TEXT(" UMenu::OnFindSessions - GOOD")));
	for (FOnlineSessionSearchResult Results : SessionSearchResults)
	{
		FString SettingsValue;
		Results.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Results);
			return;
		}
	}
	ScreenMessage(FString(TEXT(" UMenu::OnFindSessions - NOT FOUND")));
	ScreenMessage(FString::Printf(TEXT("NUM: %d, success: %d"), SessionSearchResults.Num(), bWasSuccessful));


	// if (!bWasSuccessful || SessionSearchResults.Num() == 0)
	// {
		JoinButton->SetIsEnabled(true);
	// }
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	ScreenMessage(FString(TEXT("OnJoinSession")));
	if (IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get())
	{

		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			if (APlayerController *PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	UE_LOG(LogTemp, Error, TEXT("HOST"));
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);

	}
}

void UMenu::JoinButtonClicked()
{
	UE_LOG(LogTemp, Error, TEXT("JOIN"));
	ScreenMessage(FString(TEXT("UMenu::JoinButtonClicked")));
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		ScreenMessage(FString(TEXT("UMenu::JoinButtonClicked - MultiplayerSessionsSubsystem")));

		MultiplayerSessionsSubsystem->FindSession(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	if (GetWorld())
	{
		APlayerController *PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
	}
	
	return true;
}

void UMenu::ScreenMessage(FString Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
		-1,
		15.f,
		FColor::Red,
		Message);
	}
}
