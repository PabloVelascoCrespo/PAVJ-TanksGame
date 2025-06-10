#include "LobbyWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyPlayerState.h"
#include "LobbyPlayerController.h"
#include "Components/Button.h"
#include "TankPreviewActor.h"

void ULobbyWidget::UpdatePlayerList()
{
}

void ULobbyWidget::SetReady(bool bIsReady)
{
  if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
  {
    LPC->Server_SetReadyStatus(bIsReady);

    SkinSelector->SetIsEnabled(!bIsReady);
  }
}

void ULobbyWidget::OnSkinSelected(const FString& SelectedSkin)
{
  int32 SkinIndex = 0;

  if (SelectedSkin == "Verde")
  {
    SkinIndex = 0;
  }
  else if (SelectedSkin == "Camuflaje")
  {
    SkinIndex = 1;
  }
  else if (SelectedSkin == "Arena")
  {
    SkinIndex = 2;
  }
  if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
  {
    PC->Server_SetSkinIndex(SkinIndex);
    UpdateTankPreviewSkin(SkinIndex);
  }
}

void ULobbyWidget::OnMapSelected(const FString& SelectedMap)
{
}

void ULobbyWidget::RequestStartGame(FName SelectedMap)
{
  if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
  {
    LPC->Server_RequestStartGame(SelectedMap);
  }
}

void ULobbyWidget::SetTankPreviewActor(ATankPreviewActor* PreviewActor)
{
  TankPreview = PreviewActor;
}

void ULobbyWidget::NativeConstruct()
{
  Super::NativeConstruct();

  FTimerHandle RefreshHandle;
  GetWorld()->GetTimerManager().SetTimer(RefreshHandle, this, &ULobbyWidget::RefreshScrollBox, 1.0f, true);

  if (IsValid(SkinSelector) && SkinSelector->GetOptionCount() > 0)
  {
    FString FirstOption = SkinSelector->GetOptionAtIndex(0);
    SkinSelector->SetSelectedOption(FirstOption);
  }

  if (IsValid(MapSelector) && MapSelector->GetOptionCount() > 0)
  {
    FString FirstOption = MapSelector->GetOptionAtIndex(0);
    MapSelector->SetSelectedOption(FirstOption);
  }

  if (APlayerController* PC = GetOwningPlayer())
  {
    if (!PC->HasAuthority())
    {
      if (MapSelector)
      {
        MapSelector->SetVisibility(ESlateVisibility::Collapsed);
      }
      if (StartGameButton)
      {
        StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
      }
    }
  }
}

void ULobbyWidget::RefreshScrollBox()
{
  if (!IsValid(PlayersListScrollBox))
  {
    return;
  }

  PlayersListScrollBox->ClearChildren();

  if (const UWorld* World = GetWorld())
  {
    if (const AGameStateBase* GameState = World->GetGameState())
    {
      for (APlayerState* PS : GameState->PlayerArray)
      {
        const ALobbyPlayerState* LPS = Cast<ALobbyPlayerState>(PS);
        if (LPS)
        {
          const FString Name = LPS->GetPlayerName();
          const FString Status = LPS->bIsReady ? TEXT("LISTO") : TEXT("ESPERANDO");

          UTextBlock* TextEntry = NewObject<UTextBlock>(this);
          TextEntry->SetText(FText::FromString(Name + " - " + Status));

          PlayersListScrollBox->AddChild(TextEntry);
        }
        else
        {
          UE_LOG(LogTemp, Warning, TEXT("LobbyWidget: Player State is not LobbyPlayerState"));
        }
      }
    }
    else
    {
      UE_LOG(LogTemp, Warning, TEXT("LobbyWidget has no access to the GameState"));
    }
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("Lobby Widget has no access to World!"));
  }
}

void ULobbyWidget::UpdateTankPreviewSkin(int32 SkinIndex)
{
  if (TankPreview)
  {
    TankPreview->ApplySkinByIndex(SkinIndex);
  }
}
