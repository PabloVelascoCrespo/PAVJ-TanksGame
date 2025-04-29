#include "Tank.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Projectile.h"
#include "Net/UnrealNetwork.h"

ATank::ATank()
{
  // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  m_pTankBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Body"));
  RootComponent = m_pTankBody;

  m_pTankTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Turret"));
  m_pTankTurret->SetupAttachment(m_pTankBody);

  m_pTankCannon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Cannon"));
  m_pTankCannon->SetupAttachment(m_pTankTurret);

  m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
  m_pSpringArm->SetupAttachment(m_pTankBody);
  m_pSpringArm->bUsePawnControlRotation = false;

  m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  m_pCamera->SetupAttachment(m_pSpringArm);
  m_pCamera->bUsePawnControlRotation = false;

  bReplicates = true;
  SetReplicateMovement(true);
}

void ATank::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if(IsLocallyControlled())
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

  DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 120.0f), RoleString, this, FColor::Green, DeltaTime);
}

void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
  {
    Input->BindAction(m_pMoveForwardAction, ETriggerEvent::Triggered, this, &ATank::MoveForward);
    Input->BindAction(m_pMoveForwardAction, ETriggerEvent::Completed, this, &ATank::MoveForward);
    Input->BindAction(m_pLookUpAction, ETriggerEvent::Triggered, this, &ATank::LookUp);
    Input->BindAction(m_pTurnAction, ETriggerEvent::Triggered, this, &ATank::Turn);
    Input->BindAction(m_pFireAction, ETriggerEvent::Triggered, this, &ATank::Fire);
    Input->BindAction(m_pRotateTurretAction, ETriggerEvent::Triggered, this, &ATank::TurnView);
  }
}

void ATank::BeginPlay()
{
  Super::BeginPlay();
  m_pPC = Cast<APlayerController>(GetController());
  if (m_pPC)
  {
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(m_pPC->GetLocalPlayer()))
    {
      Subsystem->AddMappingContext(m_pInputMaping, 0);
    }

    if (CrosshairWidgetClass)
    {
      UUserWidget* Crosshair = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);

      if (Crosshair)
      {
        Crosshair->AddToViewport();
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
}


void ATank::MoveForward(const FInputActionValue& Value)
{
  MoveInputValue = Value.Get<float>();
  
  if(!HasAuthority())
  {
    Server_MoveForward(MoveInputValue);
  }
}

void ATank::ForwardMovement(float DeltaTime)
{
  float InputToUse = (IsLocallyControlled() || HasAuthority()) ? MoveInputValue : MoveInputValueReplicated;

  float TargetSpeed = InputToUse * m_fMaxMoveSpeed;
  
  if (InputToUse != 0)
  {
    m_fMoveSpeed = FMath::FInterpTo(m_fMoveSpeed, TargetSpeed, DeltaTime, m_fAccelerationRate);
  }
  else
  {
    m_fMoveSpeed = FMath::FInterpTo(m_fMoveSpeed, 0.0f, DeltaTime, m_fDecelerationRateRate);

  }

  if (m_fMoveSpeed != 0)
  {
    FVector Forward = FVector::ForwardVector;
    AddActorLocalOffset(Forward * m_fMoveSpeed * DeltaTime, true);
  }
}

void ATank::Turn(const FInputActionValue& Value)
{
  float InputValue = Value.Get<float>();

  AddActorLocalRotation(FRotator(0.f, InputValue * m_fTurnSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));

  if (!HasAuthority())
  {
    Server_Turn(InputValue);
  }
}

void ATank::TurnView(const FInputActionValue& Value)
{
  float InputValue = Value.Get<float>();

  if (InputComponent != 0 && m_pSpringArm)
  {
    FRotator NewRotation = m_pSpringArm->GetRelativeRotation();
    NewRotation.Yaw += InputValue * m_fTurnViewSpeed * GetWorld()->GetDeltaSeconds();
    m_pSpringArm->SetRelativeRotation(NewRotation);

    UE_LOG(LogTemp, Warning, TEXT("Turning View"));

  }
}

void ATank::LookUp(const FInputActionValue& Value)
{
  float InputValue = Value.Get<float>();

  if (InputComponent != 0 && m_pSpringArm)
  {
    FRotator NewRotation = m_pSpringArm->GetRelativeRotation();
    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + InputValue * m_fLookUpSpeed * GetWorld()->GetDeltaSeconds(), m_fMinLookUpPitch, m_fMaxLookUpPitch);
    m_pSpringArm->SetRelativeRotation(NewRotation);

    UE_LOG(LogTemp, Warning, TEXT("Looking Up"));
  }
}

void ATank::Fire(const FInputActionValue& Value)
{
  if (!Value.Get<bool>())
  {
    return;
  }

  float CurrentTime = GetWorld()->GetTimeSeconds();

  if (CurrentTime - LastFireTime >= m_fFireRate)
  {
    LastFireTime = CurrentTime;

    if (ProjectileClass && m_pTankCannon)
    {
      FVector SpawnLocation = m_pTankCannon->GetComponentLocation() + m_pTankCannon->GetForwardVector() * MuzzleOffset;
      FRotator SpawnRotation = m_pTankCannon->GetComponentRotation();

      FActorSpawnParameters SpawnParams;
      SpawnParams.Owner = this;
      SpawnParams.Instigator = GetInstigator();

      GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

      if (m_pPC)
      {
        m_pPC->ClientStartCameraShake(FireCameraShake);
      }
    }
  }
}

void ATank::RotateTurret()
{
  if (!m_pTankTurret)
  {
    return;
  }

  FRotator DesiredRotation = FRotator(0.0f, m_pCamera->GetComponentRotation().Yaw, 0.0f);
  FRotator CurrentRotation = m_pTankTurret->GetComponentRotation();

  float DeltaTime = GetWorld()->GetDeltaSeconds();

  FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, m_fTurretRotationSpeed);
  m_pTankTurret->SetWorldRotation(FRotator(0.0f, SmoothedRotation.Yaw, 0.0f));

  Server_UpdateTurretRotation(m_pTankTurret->GetComponentRotation());
}

void ATank::AimCannonToCursor()
{
  if (!m_pCamera || !m_pTankCannon || !m_pPC)
  {
    return;
  }

  FVector2D ViewportSize;
  GEngine->GameViewport->GetViewportSize(ViewportSize);
  FVector2D ScreenCenter(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);

  FVector WorldLocation, WorldDirection;

  if (m_pPC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
  {
    FVector TraceStart = WorldLocation;
    FVector TraceEnd = TraceStart + (WorldDirection * 10000.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
    {
      FVector AimTarget = Hit.ImpactPoint;

      FVector CannonLocation = m_pTankCannon->GetComponentLocation();
      FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(CannonLocation, AimTarget);

      float ClampedPitch = FMath::Clamp(DesiredRotation.Pitch, m_fMinCannonPitch, m_fMaxCannonPitch);

      FRotator CurrentRotation = m_pTankCannon->GetRelativeRotation();
      FRotator TargetRotation = FRotator(ClampedPitch, 0.0f, 0.0f);

      float DeltaTime = GetWorld()->GetDeltaSeconds();

      FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, m_fCannonPitchSpeed);
      m_pTankCannon->SetRelativeRotation(SmoothedRotation);

      Server_UpdateCannonPitch(SmoothedRotation.Pitch);
    }
  }
}

void ATank::UpdateCannonIndicatorWidgetPosition()
{
  if (!m_pPC || !m_pTankCannon)
  {
    return;
  }

  FVector Start = m_pTankCannon->GetComponentLocation();
  FVector End = Start + m_pTankCannon->GetForwardVector() * 10000.0f;

  FHitResult Hit;
  FCollisionQueryParams Params;
  Params.AddIgnoredActor(this);

  FVector TargetPoint = End;
  if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
  {
    TargetPoint = Hit.ImpactPoint;
  }

  FVector2D ScreenPosition;
  if (m_pPC->ProjectWorldLocationToScreen(TargetPoint, ScreenPosition))
  {
    CannonIndicatorWidget->SetPositionInViewport(ScreenPosition, true);
    CannonIndicatorWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
  }
}

// === Multiplayer ===

void ATank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);

  DOREPLIFETIME(ATank, MoveInputValueReplicated)
  DOREPLIFETIME(ATank, TurretRotationReplicated)
  DOREPLIFETIME(ATank, CannonPitchReplicated)
}

void ATank::Server_UpdateCannonPitch_Implementation(float NewPitch)
{
  CannonPitchReplicated = NewPitch;
  if (HasAuthority())
  {
    m_pTankCannon->SetRelativeRotation(FRotator(NewPitch, 0.0f, 0.0f));
  }
}

void ATank::Server_MoveForward_Implementation(float Value)
{
  MoveInputValue = Value;
  MoveInputValueReplicated = Value;
}

void ATank::Server_Turn_Implementation(float Value)
{
  AddActorLocalRotation(FRotator(0.f, Value * m_fTurnSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));
}

void ATank::Server_UpdateTurretRotation_Implementation(FRotator NewRotation)
{
  TurretRotationReplicated = NewRotation;

  if (HasAuthority())
  {
    m_pTankTurret->SetWorldRotation(TurretRotationReplicated);
  }
}

void ATank::Server_Fire_Implementation()
{
}

void ATank::OnRep_TurretRotation()
{
  if (!IsLocallyControlled())
  {
    m_pTankTurret->SetWorldRotation(TurretRotationReplicated);
  }
}

void ATank::OnRep_CannonPitch()
{

  if (!IsLocallyControlled())
  {
    m_pTankCannon->SetRelativeRotation(FRotator(CannonPitchReplicated, 0.0f, 0.0f));
  }
}
