#include "LobbyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "LobbyPlayerState.h"
#include "LobbyWidget.h"
#include "LobbyGameMode.h"
#include "GameFramework/PlayerState.h"
#include "TanksGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "TankPreviewActor.h"

void ALobbyPlayerController::BeginPlay()
{
  Super::BeginPlay();

  bShowMouseCursor = true;
  FInputModeUIOnly InputMode;
  InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
  InputMode.SetWidgetToFocus(nullptr);
  SetInputMode(InputMode);

  if (IsLocalController() && TankPreviewClass)
  {
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector SpawnLoc(0.0f, 0.0f, 0.0f);
    FRotator SpawnRot(0.0f, 0.0f, 0.0f);

    TankPreview = GetWorld()->SpawnActor<ATankPreviewActor>(TankPreviewClass, SpawnLoc, SpawnRot, SpawnParams);
  }

  if (IsLocalController() && LobbyWidgetClass)
  {
    LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
    if (IsValid(LobbyWidget))
    {
      LobbyWidget->AddToViewport();
    }
    if (IsValid(TankPreview))
    {
      LobbyWidget->SetTankPreviewActor(TankPreview);
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

void ALobbyPlayerController::Server_SetSkinIndex_Implementation(uint32 SkinIndex)
{
  if (UTanksGameInstance* GI = Cast<UTanksGameInstance>(GetGameInstance()))
  {
    if (APlayerState* PS = GetPlayerState<APlayerState>())
    {
      int32 Index = GetWorld()->GetGameState()->PlayerArray.IndexOfByKey(PS);
      GI->SavePlayerSkinByIndex(Index, SkinIndex);
    }
  }
}