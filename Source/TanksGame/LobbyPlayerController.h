
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class ULobbyWidget;
class ATankPreviewActor;

UCLASS()
class TANKSGAME_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SetReadyStatus(bool bIsReady);

	UFUNCTION(Server, Reliable)
	void Server_RequestStartGame(FName SelectedMap);

	UFUNCTION(Server, Reliable)
	void Server_SetSkinIndex(uint32 SkinIndex);

	// UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<ULobbyWidget> LobbyWidget;
protected:
private:
	UPROPERTY()
	TObjectPtr<ATankPreviewActor> TankPreview;

	UPROPERTY(EditDefaultsOnly, Category = "Skin")
	TSubclassOf<ATankPreviewActor> TankPreviewClass;
};
