#include "Tank.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

}

void ATank::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}

void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
  {
    Input->BindAction(m_pMoveForwardAction, ETriggerEvent::Triggered, this, &ATank::MoveForward);
    Input->BindAction(m_pLookUpAction, ETriggerEvent::Triggered, this, &ATank::LookUp);
    Input->BindAction(m_pTurnAction, ETriggerEvent::Triggered, this, &ATank::Turn);
    Input->BindAction(m_pFireAction, ETriggerEvent::Triggered, this, &ATank::Fire);
    Input->BindAction(m_pRotateTurretAction, ETriggerEvent::Triggered, this, &ATank::TurnView);
  }
}

void ATank::BeginPlay()
{
  Super::BeginPlay();

  if (APlayerController* PC = Cast<APlayerController>(GetController()))
  {
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
    {
      Subsystem->AddMappingContext(m_pInputMaping, 0);
    }
  }
}


void ATank::MoveForward(const FInputActionValue& Value)
{
  float InputValue = Value.Get<float>();

  if (InputComponent != 0)
  {
    FVector Forward = FVector::ForwardVector;
    AddActorLocalOffset(Forward * InputValue * m_fMoveSpeed * GetWorld()->GetDeltaSeconds(), 0.0f);
    UE_LOG(LogTemp, Warning, TEXT("Moving"));
  }
}

void ATank::Turn(const FInputActionValue& Value)
{
  float InputValue = Value.Get<float>();

  if (InputComponent != 0)
  {
    AddActorLocalRotation(FRotator(0.f, InputValue * m_fTurnSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));
    UE_LOG(LogTemp, Warning, TEXT("Turning"));
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

    RotateTurret();
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

    RotateCannon();
  }
}

void ATank::Fire(const FInputActionValue& Value)
{
}

void ATank::RotateTurret()
{
  if (!m_pTankTurret)
  {
    return;
  }

  FRotator CameraRotation = m_pCamera->GetComponentRotation();
  FRotator TurretRotation = m_pTankTurret->GetComponentRotation();

  FRotator NewRotation = FRotator(0.0f, CameraRotation.Yaw, 0.0f);
  m_pTankTurret->SetWorldRotation(NewRotation);
}

void ATank::RotateCannon()
{
  if (!m_pTankCannon)
  {
    return;
  }
  
  FRotator SpringArmRotation = m_pSpringArm->GetComponentRotation();
  FRotator TurretRotation = m_pTankTurret->GetComponentRotation();

  FRotator NewRotation = FRotator(FMath::Clamp(SpringArmRotation.Pitch, m_fMinCannonPitch, m_fMaxCannonPitch), TurretRotation.Yaw, 0.0f);
  m_pTankCannon->SetWorldRotation(NewRotation);
}
