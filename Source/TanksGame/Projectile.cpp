#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Tank.h"
// Sets default values
AProjectile::AProjectile()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = false;

  bReplicates = true;
  SetReplicateMovement(true);

  CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
  CollisionComponent->InitSphereRadius(15.0f);
  CollisionComponent->SetCollisionProfileName("BlockAll");
  CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
  RootComponent = CollisionComponent;

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetupAttachment(CollisionComponent);
  Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
  ProjectileMovement->InitialSpeed = 20000.0f;
  ProjectileMovement->MaxSpeed = 20000.0f;
  ProjectileMovement->bRotationFollowsVelocity = true;
  ProjectileMovement->bShouldBounce = false;

  InitialLifeSpan = LifeSpanSeconds;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
  Super::BeginPlay();

}

void AProjectile::Multicast_SpawnImpactEffect_Implementation(FVector Location)
{
  if (ImpactEffect)
  {
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Location, FRotator::ZeroRotator);
  }
  if (ImpactSounds.Num() > 0)
  {
    APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
    if (!PC || !PC->PlayerCameraManager)
    {
      return;
    }
    FVector ListenerLocation = PC->PlayerCameraManager->GetCameraLocation();
    float Distance = FVector::Dist(ListenerLocation, Location);

    float Volume = 1.0f;
    const float MinVolume = 0.2f;
    const float MaxDistance = 80000.0f;

    Volume = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, MaxDistance), FVector2D(1.0f, MinVolume), Distance);

    int32 RandomIndex = FMath::RandRange(0, ImpactSounds.Num() - 1);
    USoundBase* SelectedSound = ImpactSounds[RandomIndex];

    if (SelectedSound)
    {
      UGameplayStatics::PlaySoundAtLocation(this, SelectedSound, Location, Volume);
    }
  }

}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
  if (OtherActor && OtherActor != this)
  {
    if (HasAuthority())
    {
      if (ImpactEffect)
      {
        Multicast_SpawnImpactEffect(Hit.ImpactPoint);
      }
      ATank* HitTank = Cast<ATank>(OtherActor);
      if (HitTank)
      {
        HitTank->TakeDmg(Damage);
      }

      Destroy();
    }
  }
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}
