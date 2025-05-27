#include "LobbyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "LobbyPlayerState.h"
#include "LobbyWidget.h"
#include "LobbyGameMode.h"
#include "GameFramework/PlayerState.h"

void ALobbyPlayerController::BeginPlay()
{
  Super::BeginPlay();

  if (IsLocalController() && LobbyWidgetClass)
  {
    LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
    if (IsValid(LobbyWidget))
    {
      LobbyWidget->AddToViewport();
    }
  }
}

void ALobbyPlayerController::Server_RequestStartGame_Implementation(FName SelectedMap)
{
  ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
  if (GM)
  {
    GM->StartGame(SelectedMap);
  }
}

void ALobbyPlayerController::Server_SetReadyStatus_Implementation(bool bIsReady)
{
  ALobbyPlayerState* LPS = GetPlayerState<ALobbyPlayerState>();
  if (LPS)
  {
    LPS->SetReady(bIsReady);
  }
}

