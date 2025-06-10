// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

/**
 *
 */
UCLASS()
class TANKSGAME_API ALobbyPlayerState : public APlayerState
{
  GENERATED_BODY()
public:
  UPROPERTY(ReplicatedUsing = OnRep_IsReady)
  bool bIsReady = false;

  void SetReady(bool bIsReady);
protected:
  virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

  UFUNCTION()
  void OnRep_IsReady();
private:
};
