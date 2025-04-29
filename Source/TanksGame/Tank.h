// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UUserWidget;
class UCameraShakeBase;
class AProjectile;

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

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

private:

  APlayerController* m_pPC;
  // COMPONENTS

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Tank Body"))
  UStaticMeshComponent* m_pTankBody;

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Tank Turret"))
  UStaticMeshComponent* m_pTankTurret;

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Tank Cannon"))
  UStaticMeshComponent* m_pTankCannon;

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Spring Arm"))
  USpringArmComponent* m_pSpringArm;

  UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Camera"))
  UCameraComponent* m_pCamera;

  // INPUT
  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Input Map"))
  UInputMappingContext* m_pInputMaping;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Move Forward Action"))
  UInputAction* m_pMoveForwardAction;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Turn Action"))
  UInputAction* m_pTurnAction;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Rotate Turret Action"))
  UInputAction* m_pRotateTurretAction;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Look Up Action"))
  UInputAction* m_pLookUpAction;

  UPROPERTY(EditAnywhere, Category = "Input", meta = (DisplayName = "Fire Action"))
  UInputAction* m_pFireAction;

  // MOVEMENT
  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Move Speed"))
  float m_fMoveSpeed = 300.0f;

  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Max Move Speed"))
  float m_fMaxMoveSpeed = 300.0f;

  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Acceleration Rate"))
  float m_fAccelerationRate = 4.0f;

  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Deceleration Rate"))
  float m_fDecelerationRateRate = 5.0f;

  UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Turn Speed"))
  float m_fTurnSpeed = 30.0f;

  float MoveInputValue;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Turn View Speed"))
  float m_fTurnViewSpeed = 50.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Look Up Speed"))
  float m_fLookUpSpeed = 50.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Min Cannon Pitch"))
  float m_fMinCannonPitch = -10.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Max Cannon Pitch"))
  float m_fMaxCannonPitch = 20.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Min Camera Pitch"))
  float m_fMinLookUpPitch = -50.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Max Camera Pitch"))
  float m_fMaxLookUpPitch = 0.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Turret Rotation Speed"))
  float m_fTurretRotationSpeed = 15.0f;

  UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Cannon Rotation Speed"))
  float m_fCannonPitchSpeed = 15.0f;

  UPROPERTY(EditAnywhere, Category = "Combat", meta = (DisplayName = "Fire Rate"))
  float m_fFireRate = 0.7f;

  float LastFireTime = -INFINITY;

  UPROPERTY(EditAnywhere, Category = "Combat")
  TSubclassOf<AProjectile> ProjectileClass;

  UPROPERTY(EditAnywhere, Category = "Combat")
  float MuzzleOffset = 540.0f;

  UPROPERTY(EditAnywhere, Category = "Combat")
  TSubclassOf<UCameraShakeBase> FireCameraShake;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> CrosshairWidgetClass;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> CannonIndicatorWidgetClass;
  UUserWidget* CannonIndicatorWidget;

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

};
