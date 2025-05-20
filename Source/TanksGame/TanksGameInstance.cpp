#include "TanksGameInstance.h"

void UTanksGameInstance::SavePlayerSkin(const FString& PlayerName, int32 SkinIndex)
{
  PlayerSkins.Add(PlayerName, SkinIndex);
}

int32 UTanksGameInstance::GetPlayerSkin(const FString& PlayerName) const
{
  const int32* Found = PlayerSkins.Find(PlayerName);
  return 0;
  //return Found ? *Found : 0;
}
