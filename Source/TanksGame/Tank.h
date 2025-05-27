// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UCameraShakeBase;
class AProjectile;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class TANKSGAME_API ATank : public APawn
{
  GENERATED_BODY()

public:
  // Sets default values for this pawn's properties
  ATank();
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  // Called to bind functionality to input
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

  void TakeDmg(float Damage);

  UFUNCTION()
  void ApplySkinByIndex(int32 Index);

  UPROPERTY(Replicated)
  int32 PlayerIndex;
protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  virtual void PossessedBy(AController* NewController) override;

private:

  void InitializeLocalPlayerUI();

  UPROPERTY(Replicated, EditAnywhere, Category = "Stats")
  float MaxHealth = 100.0f;

  UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
  float CurrentHealth;

  UFUNCTION()
  void OnRep_CurrentHealth();

  void HandleDestruction();

  TObjectPtr<APlayerController> PC;
  // COMPONENTS

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Tank Body"))
  TObjectPtr<UStaticMeshComponent> TankBody;

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Tank Turret"))
  TObjectPtr<UStaticMeshComponent> TankTurret;

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Tank Cannon"))
  TObjectPtr<UStaticMeshComponent> TankCannon;

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Spring Arm"))
  TObjectPtr<USpringArmComponent> SpringArm;

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Camera"))
  TObjectPtr<UCameraComponent> Camera;

  // INPUT
  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Input Map"))
  TObjectPtr<UInputMappingContext> InputMaping;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Move Forward Action"))
  TObjectPtr<UInputAction> MoveForwardAction;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Turn Action"))
  TObjectPtr<UInputAction> TurnAction;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Rotate Turret Action"))
  TObjectPtr<UInputAction> RotateTurretAction;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Look Up Action"))
  TObjectPtr<UInputAction> LookUpAction;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Fire Action"))
  TObjectPtr<UInputAction> FireAction;

  // MOVEMENT
  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Move Speed"))
  float MoveSpeed = 300.0f;

  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Max Move Speed"))
  float MaxMoveSpeed = 300.0f;

  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Acceleration Rate"))
  float AccelerationRate = 4.0f;

  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Deceleration Rate"))
  float DecelerationRateRate = 5.0f;

  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Turn Speed"))
  float TurnSpeed = 30.0f;

  float MoveInputValue;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Turn View Speed"))
  float TurnViewSpeed = 50.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Look Up Speed"))
  float LookUpSpeed = 50.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Min Cannon Pitch"))
  float MinCannonPitch = -10.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Max Cannon Pitch"))
  float MaxCannonPitch = 20.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Min Camera Pitch"))
  float MinLookUpPitch = -50.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Max Camera Pitch"))
  float MaxLookUpPitch = 0.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Turret Rotation Speed"))
  float TurretRotationSpeed = 15.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Cannon Rotation Speed"))
  float CannonPitchSpeed = 15.0f;

  UPROPERTY(EditAnywhere, Category = "Combat", meta = (DisplayName = "Fire Rate"))
  float FireRate = 0.7f;

  float LastFireTime = -INFINITY;

  UPROPERTY(EditAnywhere, Category = "Combat")
  TSubclassOf<AProjectile> ProjectileClass;

  UPROPERTY(EditAnywhere, Category = "Combat")
  float MuzzleOffset = 560.0f;

  // ===== UI =====
  UPROPERTY(EditAnywhere, Category = "Combat")
  TSubclassOf<UCameraShakeBase> FireCameraShake;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> CrosshairWidgetClass;
  TObjectPtr<UUserWidget> CrosshairWidget;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> CannonIndicatorWidgetClass;
  TObjectPtr<UUserWidget> CannonIndicatorWidget;
  // ===== UI =====

  void MoveForward(const FInputActionValue& Value);
  void ForwardMovement(float DeltaTime);

  void Turn(const FInputActionValue& Value);
  void TurnView(const FInputActionValue& Value);
  void LookUp(const FInputActionValue& Value);
  void Fire(const FInputActionValue& Value);

  void RotateTurret();
  void AimCannonToCursor();
  void UpdateCannonIndicatorWidgetPosition();


  // === Multiplayer ===
  virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

  UFUNCTION(Server, Reliable)
  void Server_MoveForward(float Value);

  UFUNCTION(Server, Reliable)
  void Server_Turn(float Value);

  UFUNCTION(Server, Reliable)
  void Server_UpdateTurretRotation(FRotator NewRotation);

  UFUNCTION(Server, Reliable)
  void Server_UpdateCannonPitch(float NewPitch);

  UFUNCTION(Server, Reliable)
  void Server_Fire();

  UFUNCTION(Server, Reliable)
  void Client_PlayCameraShake();

  void ExecuteFire();

  // Variables Replicadas
  UPROPERTY(Replicated)
  float MoveInputValueReplicated;

  UPROPERTY(ReplicatedUsing = OnRep_TurretRotation)
  FRotator TurretRotationReplicated;

  UPROPERTY(ReplicatedUsing = OnRep_CannonPitch)
  float CannonPitchReplicated;

  UFUNCTION()
  void OnRep_TurretRotation();

  UFUNCTION()
  void OnRep_CannonPitch();

  // health system

  void UpdateHealthUI();

  UPROPERTY(VisibleAnywhere, Category = "UI")
  TObjectPtr<UWidgetComponent> TankHealthWidgetComponent;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> TankHealthBarClass;

  UPROPERTY(EditDefaultsOnly, Category = "Skins")
  TObjectPtr<UMaterialInterface> SkinMaterial0;

  UPROPERTY(EditDefaultsOnly, Category = "Skins")
  TObjectPtr<UMaterialInterface>  SkinMaterial1;

  UPROPERTY(EditDefaultsOnly, Category = "Skins")
  TObjectPtr<UMaterialInterface> SkinMaterial2;

  UPROPERTY(ReplicatedUsing = OnRep_SkinIndex)
  int32 SkinIndex;

  UFUNCTION()
  void OnRep_SkinIndex();

};
