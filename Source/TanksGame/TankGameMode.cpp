#include "TankGameMode.h"
#include "Tank.h"

void ATankGameMode::RequestRespawn(AController* PlayerController)
{
  if (!PlayerController)
  {
    return;
  }

  FTimerHandle RespawnHandle;
  GetWorld()->GetTimerManager().SetTimer(RespawnHandle, [this, PlayerController]()
    {
      RestartPlayer(PlayerController);
      FTimerHandle AssingIndexHandle;
      GetWorld()->GetTimerManager().SetTimer(AssingIndexHandle, [this, PlayerController]()
        {
          ATank* Tank = Cast<ATank>(PlayerController->GetPawn());
          if (Tank)
          {
            PlayerIndex = ConnectedPlayers.IndexOfByKey(PlayerController);
            Tank->PlayerIndex = PlayerIndex;
          }
        }, 0.2f, false);
    }, 5.0f, false);

}

AActor* ATankGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
  if (!Player)
  {
    return Super::ChoosePlayerStart_Implementation(Player);
  }

  if (!ConnectedPlayers.Contains(Player))
  {
    ConnectedPlayers.Add(Player);
  }

  PlayerIndex = ConnectedPlayers.IndexOfByKey(Player);
  FString TagToSearch = FString::FromInt(PlayerIndex);

  AActor* FoundStart = FindPlayerStart(Player, TagToSearch);

  if (FoundStart)
  {
    return FoundStart;
  }

  return Super::ChoosePlayerStart_Implementation(Player);
}

void ATankGameMode::PostLogin(APlayerController* NewPlayer)
{
  Super::PostLogin(NewPlayer);

  APawn* PlayerPawn = NewPlayer->GetPawn();
  if (!PlayerPawn)
  {
    FTimerHandle TimeHandler;
    GetWorld()->GetTimerManager().SetTimer(TimeHandler, [this, NewPlayer]()
      {
        ATank* Tank = Cast<ATank>(NewPlayer->GetPawn());
        if (Tank)
        {
          Tank->PlayerIndex = PlayerIndex;
        }
      }, 0.2f, false);
  }
  else
  {
    ATank* Tank = Cast<ATank>(NewPlayer->GetPawn());
    if (Tank)
    {
      Tank->PlayerIndex = PlayerIndex;
    }
  }
}
