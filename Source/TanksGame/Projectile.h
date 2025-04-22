// Fill out your copyright notice in the Description page of Project Settings.

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

private:
  UPROPERTY(VisibleAnywhere)
  USphereComponent* CollisionComponent;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* Mesh;

  UPROPERTY(VisibleAnywhere)
  UProjectileMovementComponent* ProjectileMovement;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float LifeSpanSeconds;

  UPROPERTY(EditDefaultsOnly, Category = "Effects")
  UParticleSystem* ImpactEffect;

  UFUNCTION()
  void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
