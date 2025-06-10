#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

UCLASS()
class TANKSGAME_API ATankPlayerController : public APlayerController
{
  GENERATED_BODY()
public:
  TObjectPtr<UUserWidget> CrosshairWidget;
  TObjectPtr<UUserWidget> CannonIndicatorWidget;

protected:
  virtual void BeginPlay() override;
private:

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> CrosshairWidgetClass;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> CannonIndicatorWidgetClass;

  void InitializeHUDWidgets();
};
