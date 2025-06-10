#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class TANKSGAME_API AProjectile : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AProjectile();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  UPROPERTY(EditDefaultsOnly, Category = "Audio")
  TArray<TObjectPtr<USoundBase>> ImpactSounds;

private:
  UPROPERTY(VisibleAnywhere)
  TObjectPtr<USphereComponent>  CollisionComponent;

  UPROPERTY(VisibleAnywhere)
  TObjectPtr<UStaticMeshComponent> Mesh;

  UPROPERTY(VisibleAnywhere)
  TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float LifeSpanSeconds;

  UPROPERTY(EditAnywhere, Category = "Combat")
  float Damage = 20.0f;

  UPROPERTY(EditDefaultsOnly, Category = "Effects")
  TObjectPtr<UParticleSystem> ImpactEffect;

  UFUNCTION(NetMulticast, Reliable)
  void Multicast_SpawnImpactEffect(FVector Location);

  UFUNCTION()
  void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

  virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
