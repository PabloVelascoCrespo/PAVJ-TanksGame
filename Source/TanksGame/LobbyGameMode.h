#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class TANKSGAME_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StartGame(FName MapName);

	bool AreAllPlayerReady() const;
protected:

private:
	
};
