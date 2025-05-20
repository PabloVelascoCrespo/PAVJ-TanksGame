#include "TankGameMode.h"
#include "Tank.h"
#include "GameFramework/PlayerState.h"
#include "TanksGameInstance.h"

void ATankGameMode::RequestRespawn(AController* PlayerController)
{
  if (!PlayerController)
  {
    return;
  }

  FTimerHandle RespawnHandle;
  GetWorld()->GetTimerManager().SetTimer(RespawnHandle, 
    [this, PlayerController]()
    {
      RestartPlayer(PlayerController);
      
      //AssignPlayerIndexToPawn(Cast<APlayerController>(PlayerController));

      FTimerHandle AssingIndexHandle;
      GetWorld()->GetTimerManager().SetTimer(AssingIndexHandle, 
        [this, PlayerController]()
        {
          ATank* Tank = Cast<ATank>(PlayerController->GetPawn());
          if (Tank)
          {
            PlayerIndex = ConnectedPlayers.IndexOfByKey(PlayerController);
            Tank->PlayerIndex = PlayerIndex;
            ApplySkinToTank(PlayerController, Tank);
          }
        }, 0.2f, false);
    }, 5.0f, false);

}

void ATankGameMode::ApplySkinToTank(AController* Controller, ATank* Tank)
{
  if (!Controller || !Tank)
  {
    return;
  }

  UTanksGameInstance* GI = Cast<UTanksGameInstance>(GetGameInstance());
  if (GI)
  {
    const FString PlayerName = Controller->PlayerState->GetPlayerName();
    const int32 SkinIndex = GI->GetPlayerSkin(PlayerName);
    Tank->ApplySkinByIndex(SkinIndex);
  }
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

  //AssignPlayerIndexToPawn(NewPlayer);

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
          ApplySkinToTank(NewPlayer, Tank);
        }
      }, 0.2f, false);
  }
  else
  {
    ATank* Tank = Cast<ATank>(NewPlayer->GetPawn());
    if (Tank)
    {
      Tank->PlayerIndex = PlayerIndex;
      ApplySkinToTank(NewPlayer, Tank);
    }
  }
}

void ATankGameMode::AssignPlayerIndexToPawn(APlayerController* PlayerController)
{
  if (!PlayerController || !PlayerController->PlayerState)
  {
    return;
  }

  PlayerIndex = PlayerController->PlayerState->GetPlayerId();
  APawn* PlayerPawn = PlayerController->GetPawn();
  if (PlayerPawn)
  {
    ATank* Tank = Cast<ATank>(PlayerPawn);
    if (Tank)
    {
      Tank->PlayerIndex = PlayerIndex;
    }
  }
  else
  {
    FTimerHandle AssignIndexHandle;
    GetWorld()->GetTimerManager().SetTimer(AssignIndexHandle,
      [this, PlayerController]()
      {
        AssignPlayerIndexToPawn(PlayerController);
      }, 0.2f, false);
  }
}
