// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"

void ALobbyPlayerState::SetReady(bool _bIsReady)
{
  bIsReady = _bIsReady;
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);
  DOREPLIFETIME(ALobbyPlayerState, bIsReady);
}

void ALobbyPlayerState::OnRep_IsReady()
{

}
