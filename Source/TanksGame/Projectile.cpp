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
  UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Location, FRotator::ZeroRotator);

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
