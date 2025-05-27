// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UScrollBox;
class UComboBoxString;
class UButton;

UCLASS()
class TANKSGAME_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerList();

	UFUNCTION(BlueprintCallable)
	void SetReady(bool bIsReady);

	UFUNCTION (BlueprintCallable)
	void OnSkinSelected(const FString& SelectedSking);

	UFUNCTION(BlueprintCallable)
	void OnMapSelected(const FString& SelectedMap);

	UFUNCTION(BlueprintCallable)
	void RequestStartGame(FName SelectedMap);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> PlayersListScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> SkinSelector;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> MapSelector;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

protected:
	virtual void NativeConstruct() override;

	void RefreshScrollBox();
private:
};
