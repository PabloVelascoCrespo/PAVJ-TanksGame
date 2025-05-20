#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TanksGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TANKSGAME_API UTanksGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	void SavePlayerSkin(const FString& PlayerName, int32 SkinIndex);

	int32 GetPlayerSkin(const FString& PlayerName) const;
protected:
	
private:
	UPROPERTY()
	TMap<FString, int32> PlayerSkins;
};
