#include "Tank.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "TankGameMode.h"
#include "TankPlayerController.h"

ATank::ATank()
{
  // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  TankBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Body"));
  RootComponent = TankBody;

  TankTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Turret"));
  TankTurret->SetupAttachment(TankBody);

  TankCannon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Cannon"));
  TankCannon->SetupAttachment(TankTurret);

  SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
  SpringArm->SetupAttachment(TankBody);
  SpringArm->bUsePawnControlRotation = false;

  Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  Camera->SetupAttachment(SpringArm);
  Camera->bUsePawnControlRotation = false;

  // HEALTH
  TankHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TankHealthWidget"));
  TankHealthWidgetComponent->SetupAttachment(TankBody);
  TankHealthWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

  bReplicates = true;
  SetReplicateMovement(true);
}

void ATank::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (IsLocallyControlled())
  {
    RotateTurret();
    AimCannonToCursor();
  }
  UpdateCannonIndicatorWidgetPosition();
  ForwardMovement(DeltaTime);

  /*********************TEST**********************/
  FString RoleString;
  switch (GetLocalRole())
  {
  case ROLE_Authority:
    RoleString = "Authority";
    break;
  case ROLE_AutonomousProxy:
    RoleString = "AutonomousProxy";
    break;
  case ROLE_SimulatedProxy:
    RoleString = "Simulated Proxy";
    break;
  default:
    RoleString = "Unknown Role";
    break;
  }

  FString Label = FString::Printf(TEXT("%s - Player: %d"), *RoleString, PlayerIndex);

  DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 120.0f), Label, this, FColor::Green, DeltaTime);
}

void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
  {
    Input->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ATank::MoveForward);
    Input->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ATank::MoveForward);
    Input->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &ATank::LookUp);
    Input->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ATank::Turn);
    Input->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATank::Fire);
    Input->BindAction(RotateTurretAction, ETriggerEvent::Triggered, this, &ATank::TurnView);
  }
}

void ATank::TakeDmg(float Damage)
{
  if (!HasAuthority())
  {
    return;
  }

  CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

  if (CurrentHealth <= 0.0f)
  {
    HandleDestruction();
  }

  UpdateHealthUI();
}

void ATank::ApplySkinByIndex(int32 Index)
{
  SkinIndex = Index;

  if (!TankBody || !TankTurret || !TankCannon)
  {
    return;
  }

  UMaterialInterface* SelectedMaterial = nullptr;

  switch (Index)
  {
  case 0:
    SelectedMaterial = SkinMaterial0;
    break;
  case 1:
    SelectedMaterial = SkinMaterial1;
    break;
  case 2:
    SelectedMaterial = SkinMaterial2;
    break;
  default:
    SelectedMaterial = SkinMaterial0;
    break;
  }

  if (SelectedMaterial)
  {
    TankBody->SetMaterial(0, SelectedMaterial);
    TankTurret->SetMaterial(0, SelectedMaterial);
    TankCannon->SetMaterial(0, SelectedMaterial);
  }
}


void ATank::OnRep_CurrentHealth()
{
  UpdateHealthUI();
}

void ATank::HandleDestruction()
{
  AController* TankController = GetController();
  DetachFromControllerPendingDestroy();
  Destroy();

  if (HasAuthority() && TankController)
  {
    ATankGameMode* GameMode = Cast<ATankGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
      GameMode->RequestRespawn(TankController);
    }
  }
}

void ATank::BeginPlay()
{
  Super::BeginPlay();

  if (IsLocallyControlled())
  {
    InitializeLocalPlayerUI();
  }

  CurrentHealth = MaxHealth;

  UpdateHealthUI();
}

void ATank::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  if (CrosshairWidget)
  {
    CrosshairWidget->RemoveFromParent();
    CrosshairWidget = nullptr;
  }

  if (CannonIndicatorWidget)
  {
    CannonIndicatorWidget->RemoveFromParent();
    CannonIndicatorWidget = nullptr;
  }

}

void ATank::PossessedBy(AController* NewController)
{
  Super::PossessedBy(NewController);
  if (IsLocallyControlled())
  {
    InitializeLocalPlayerUI();
  }
}

void ATank::InitializeLocalPlayerUI()
{
  PC = Cast<APlayerController>(GetController());
  if (PC)
  {
    PC->bShowMouseCursor = false;
    PC->SetInputMode(FInputModeGameOnly());

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
    {
      Subsystem->AddMappingContext(InputMaping, 0);
    }

    if (CrosshairWidgetClass && !CrosshairWidget)
    {
      CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);

      if (CrosshairWidget)
      {
        CrosshairWidget->AddToViewport();
      }
    }

    if (CannonIndicatorWidgetClass && !CannonIndicatorWidget)
    {
      CannonIndicatorWidget = CreateWidget<UUserWidget>(GetWorld(), CannonIndicatorWidgetClass);

      if (CannonIndicatorWidget)
      {
        CannonIndicatorWidget->AddToViewport();
      }
    }

    if (TankHealthWidgetComponent)
    {
      TankHealthWidgetComponent->SetVisibility(false);
    }
  }
}

void ATank::MoveForward(const FInputActionValue& Value)
{
  MoveInputValue = Value.Get<float>();

  if (!HasAuthority())
  {
    Server_MoveForward(MoveInputValue);
  }
}

void ATank::ForwardMovement(float DeltaTime)
{
  float InputToUse = (IsLocallyControlled() || HasAuthority()) ? MoveInputValue : MoveInputValueReplicated;

  float TargetSpeed = InputToUse * MaxMoveSpeed;

  if (InputToUse != 0)
  {
    MoveSpeed = FMath::FInterpTo(MoveSpeed, TargetSpeed, DeltaTime, AccelerationRate);
  }
  else
  {
    MoveSpeed = FMath::FInterpTo(MoveSpeed, 0.0f, DeltaTime, DecelerationRateRate);

  }

  if (MoveSpeed != 0)
  {
    FVector Forward = FVector::ForwardVector;
    AddActorLocalOffset(Forward * MoveSpeed * DeltaTime, true);
  }
}

void ATank::Turn(const FInputActionValue& Value)
{
  float InputValue = Value.Get<float>();

  AddActorLocalRotation(FRotator(0.f, InputValue * TurnSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));

  if (!HasAuthority())
  {
    Server_Turn(InputValue);
  }
}

void ATank::TurnView(const FInputActionValue& Value)
{
  float InputValue = Value.Get<float>();

  if (InputComponent != 0 && SpringArm)
  {
    FRotator NewRotation = SpringArm->GetRelativeRotation();
    NewRotation.Yaw += InputValue * TurnViewSpeed * GetWorld()->GetDeltaSeconds();
    SpringArm->SetRelativeRotation(NewRotation);

    UE_LOG(LogTemp, Warning, TEXT("Turning View"));

  }
}

void ATank::LookUp(const FInputActionValue& Value)
{
  float InputValue = Value.Get<float>();

  if (InputComponent != 0 && SpringArm)
  {
    FRotator NewRotation = SpringArm->GetRelativeRotation();
    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + InputValue * LookUpSpeed * GetWorld()->GetDeltaSeconds(), MinLookUpPitch, MaxLookUpPitch);
    SpringArm->SetRelativeRotation(NewRotation);

    UE_LOG(LogTemp, Warning, TEXT("Looking Up"));
  }
}

void ATank::Fire(const FInputActionValue& Value)
{
  if (!Value.Get<bool>())
  {
    return;
  }

  if (IsLocallyControlled())
  {
    Client_PlayCameraShake();
  }

  if (!HasAuthority())
  {
    Server_Fire();
    return;
  }

  ExecuteFire();

}

void ATank::Server_Fire_Implementation()
{
  ExecuteFire();
}

void ATank::Client_PlayCameraShake_Implementation()
{
  if (PC)
  {
    PC->ClientStartCameraShake(FireCameraShake);
  }
}

void ATank::ExecuteFire()
{
  float CurrentTime = GetWorld()->GetTimeSeconds();

  if (CurrentTime - LastFireTime >= FireRate)
  {
    LastFireTime = CurrentTime;

    if (ProjectileClass && TankCannon)
    {
      FVector SpawnLocation = TankCannon->GetComponentLocation() + TankCannon->GetForwardVector() * MuzzleOffset;
      FRotator SpawnRotation = TankCannon->GetComponentRotation();

      FActorSpawnParameters SpawnParams;
      SpawnParams.Owner = this;
      SpawnParams.Instigator = GetInstigator();

      GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
    }
  }
}

void ATank::RotateTurret()
{
  if (!TankTurret)
  {
    return;
  }

  FRotator DesiredRotation = FRotator(0.0f, Camera->GetComponentRotation().Yaw, 0.0f);
  FRotator CurrentRotation = TankTurret->GetComponentRotation();

  float DeltaTime = GetWorld()->GetDeltaSeconds();

  FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, TurretRotationSpeed);
  TankTurret->SetWorldRotation(FRotator(0.0f, SmoothedRotation.Yaw, 0.0f));

  Server_UpdateTurretRotation(TankTurret->GetComponentRotation());
}

void ATank::AimCannonToCursor()
{
  if (!Camera || !TankCannon || !PC)
  {
    return;
  }

  FVector2D ViewportSize;
  GEngine->GameViewport->GetViewportSize(ViewportSize);
  FVector2D ScreenCenter(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);

  FVector WorldLocation, WorldDirection;

  if (PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
  {
    FVector TraceStart = WorldLocation;
    FVector TraceEnd = TraceStart + (WorldDirection * 10000.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
    {
      FVector AimTarget = Hit.ImpactPoint;

      FVector CannonLocation = TankCannon->GetComponentLocation();
      FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(CannonLocation, AimTarget);

      float ClampedPitch = FMath::Clamp(DesiredRotation.Pitch, MinCannonPitch, MaxCannonPitch);

      FRotator CurrentRotation = TankCannon->GetRelativeRotation();
      FRotator TargetRotation = FRotator(ClampedPitch, 0.0f, 0.0f);

      float DeltaTime = GetWorld()->GetDeltaSeconds();

      FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, CannonPitchSpeed);
      TankCannon->SetRelativeRotation(SmoothedRotation);

      Server_UpdateCannonPitch(SmoothedRotation.Pitch);
    }
  }
}

void ATank::UpdateCannonIndicatorWidgetPosition()
{
  if (!PC || !TankCannon)
  {
    return;
  }

  FVector Start = TankCannon->GetComponentLocation();
  FVector End = Start + TankCannon->GetForwardVector() * 10000.0f;

  FHitResult Hit;
  FCollisionQueryParams Params;
  Params.AddIgnoredActor(this);

  FVector TargetPoint = End;
  if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
  {
    TargetPoint = Hit.ImpactPoint;
  }

  FVector2D ScreenPosition;
  if (PC->ProjectWorldLocationToScreen(TargetPoint, ScreenPosition) && CannonIndicatorWidget)
  {
    CannonIndicatorWidget->SetPositionInViewport(ScreenPosition, true);
    CannonIndicatorWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
  }
}

// === Multiplayer ===

void ATank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);

  DOREPLIFETIME(ATank, MoveInputValueReplicated);
  DOREPLIFETIME(ATank, TurretRotationReplicated);
  DOREPLIFETIME(ATank, CannonPitchReplicated);
  DOREPLIFETIME(ATank, CurrentHealth);
  DOREPLIFETIME(ATank, PlayerIndex);
  DOREPLIFETIME(ATank, SkinIndex);

}

void ATank::Server_UpdateCannonPitch_Implementation(float NewPitch)
{
  CannonPitchReplicated = NewPitch;
  if (HasAuthority())
  {
    TankCannon->SetRelativeRotation(FRotator(NewPitch, 0.0f, 0.0f));
  }
}

void ATank::Server_MoveForward_Implementation(float Value)
{
  MoveInputValue = Value;
  MoveInputValueReplicated = Value;
}

void ATank::Server_Turn_Implementation(float Value)
{
  AddActorLocalRotation(FRotator(0.f, Value * TurnSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));
}

void ATank::Server_UpdateTurretRotation_Implementation(FRotator NewRotation)
{
  TurretRotationReplicated = NewRotation;

  if (HasAuthority())
  {
    TankTurret->SetWorldRotation(TurretRotationReplicated);
  }
}

void ATank::OnRep_TurretRotation()
{
  if (!IsLocallyControlled())
  {
    TankTurret->SetWorldRotation(TurretRotationReplicated);
  }
}

void ATank::OnRep_CannonPitch()
{
  if (!IsLocallyControlled())
  {
    TankCannon->SetRelativeRotation(FRotator(CannonPitchReplicated, 0.0f, 0.0f));
  }
}

void ATank::UpdateHealthUI()
{
  if (CrosshairWidget)
  {
    UProgressBar* HealthBar = Cast<UProgressBar>(CrosshairWidget->GetWidgetFromName(TEXT("PlayerHealthBar")));
    if (HealthBar)
    {
      HealthBar->SetPercent(CurrentHealth / MaxHealth);
    }
  }

  if (TankHealthWidgetComponent)
  {
    UUserWidget* TankWidget = TankHealthWidgetComponent->GetUserWidgetObject();
    if (TankWidget)
    {
      UProgressBar* TankBar = Cast<UProgressBar>(TankWidget->GetWidgetFromName(TEXT("HealthProgressBar")));
      if (TankBar)
      {
        TankBar->SetPercent(CurrentHealth / MaxHealth);
      }
    }
  }
}

void ATank::OnRep_SkinIndex()
{
  ApplySkinByIndex(SkinIndex);
}
