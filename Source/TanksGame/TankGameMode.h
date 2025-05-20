#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TankGameMode.generated.h"

class ATank;

UCLASS()
class TANKSGAME_API ATankGameMode : public AGameModeBase
{
  GENERATED_BODY()
public:
  UFUNCTION()
  void RequestRespawn(AController* PlayerController);

  void ApplySkinToTank(AController* Controller, ATank* Tank);

protected:
  virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

  virtual void PostLogin(APlayerController* NewPlayer) override;

  TArray<AController*> ConnectedPlayers;

  int32 PlayerIndex;

  void AssignPlayerIndexToPawn(APlayerController* PlayerController);
private:
};
