#include "TankGameMode.h"
#include "Tank.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
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
          if (!PlayerController)
          {
            return;
          }
          if (APawn* PAwn = PlayerController->GetPawn())
          {
            InitTankFromPlayer(PlayerController);
          }
          else
          {
            RequestRespawn(PlayerController);
          }
        }, 0.5f, false);
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


  int32 PlayerIndex = ConnectedPlayers.IndexOfByKey(Player);

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
        InitTankFromPlayer(NewPlayer);

      }, 0.1f, false);

    //  FTimerHandle TimeHandler;
    //  GetWorld()->GetTimerManager().SetTimer(TimeHandler, [this, NewPlayer]()
    //    {
    //      ATank* Tank = Cast<ATank>(NewPlayer->GetPawn());
    //      if (Tank)
    //      {
    //        Tank->PlayerIndex = PlayerIndex;
    //        ApplySkinToTank(NewPlayer, Tank);
    //      }
    //    }, 0.2f, false);
    //}
    //else
    //{
    //  ATank* Tank = Cast<ATank>(NewPlayer->GetPawn());
    //  if (Tank)
    //  {
    //    Tank->PlayerIndex = PlayerIndex;
    //    ApplySkinToTank(NewPlayer, Tank);
    //  }
    //}
  }
}
void ATankGameMode::AssignPlayerIndexToPawn(APlayerController* PlayerController)
{
  if (!PlayerController || !PlayerController->PlayerState)
  {
    return;
  }

  int32 PlayerIndex = PlayerController->PlayerState->GetPlayerId();
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

void ATankGameMode::InitTankFromPlayer(AController* Controller)
{
  if (!Controller || !Controller->PlayerState)
  {
    return;
  }

  if (UTanksGameInstance* GI = Cast<UTanksGameInstance>(GetGameInstance()))
  {
    int32 PlayerIndex = GetWorld()->GetGameState()->PlayerArray.IndexOfByKey(Controller->PlayerState);
    //int32 SkinIndex = GI->GetPlayerSkinByIndex(PlayerIndex);

    if (APawn* Pawn = Controller->GetPawn())
    {
      if (ATank* Tank = Cast<ATank>(Pawn))
      {
        Tank->PlayerIndex = PlayerIndex;
        //Tank->ApplySkinByIndex(SkinIndex);

      }
    }
  }
}
