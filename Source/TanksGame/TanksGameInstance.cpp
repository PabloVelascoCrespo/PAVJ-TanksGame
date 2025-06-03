#include "TanksGameInstance.h"

void UTanksGameInstance::SavePlayerSkin(const FString& PlayerName, int32 SkinIndex)
{
  PlayerSkins.Add(PlayerName, SkinIndex);
}

void UTanksGameInstance::SavePlayerSkinByIndex(int32 Index, int32 SkinIndex)
{
  IndexToSkins.Add(Index, SkinIndex);
}

int32 UTanksGameInstance::GetPlayerSkin(const FString& PlayerName) const
{
  const int32* Found = PlayerSkins.Find(PlayerName);
  return 0;
  //return Found ? *Found : 0;
}

int32 UTanksGameInstance::GetPlayerSkinByIndex(const int32 Index) const
{
  const int32* Found = IndexToSkins.Find(Index);
  return Found ? *Found : 0;
}
