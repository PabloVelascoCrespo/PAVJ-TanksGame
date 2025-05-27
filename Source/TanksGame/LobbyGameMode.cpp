#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.h"

void ALobbyGameMode::StartGame(FName MapName)
{
  if (!HasAuthority())
  {
    return;
  }
  
  if (!AreAllPlayerReady())
  {
    return;
  }

  FString MapPath = FString::Printf(TEXT("/Game/Maps/%s?listen"), *MapName.ToString());
  GetWorld()->ServerTravel(MapPath);
}

bool ALobbyGameMode::AreAllPlayerReady() const
{
  for (APlayerState* PS : GameState->PlayerArray)
  {
    ALobbyPlayerState* LPS = Cast<ALobbyPlayerState>(PS);
    if (!LPS || !LPS->bIsReady)
    {
      return false;
    }
  }
  return true;
}
