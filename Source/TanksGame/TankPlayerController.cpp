#include "TankPlayerController.h"
#include "Blueprint/UserWidget.h"

void ATankPlayerController::BeginPlay()
{
  Super::BeginPlay();

  //if (IsLocalController())
  //{
  //  InitializeHUDWidgets();
  //}
}

void ATankPlayerController::InitializeHUDWidgets()
{
  if (CrosshairWidgetClass)
  {
    CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);

    if (CrosshairWidget)
    {
      CrosshairWidget->AddToViewport();
    }
  }

  if (CannonIndicatorWidgetClass)
  {
    CannonIndicatorWidget = CreateWidget<UUserWidget>(GetWorld(), CannonIndicatorWidgetClass);

    if (CannonIndicatorWidget)
    {
      CannonIndicatorWidget->AddToViewport();
    }
  }
}
