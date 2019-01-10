// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "ParagonProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UParticleSystemComponent;
class AParagonExplosionEffect;

UCLASS()
class PARAGON_API AParagonProjectile : public AActor
{
	GENERATED_BODY()

	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* ProjectileMovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UParticleSystemComponent* ParticleComp;
	
public:	
	// Sets default values for this actor's properties
	AParagonProjectile();

	/** setup stats */
	void Init(float Lifetime, float NewExplosionRadius);

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection);

	/** handle hit */
	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

	/** effects for explosion */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class AParagonExplosionEffect> ExplosionTemplate;

	/** trigger explosion */
	void Explode(const FHitResult& Impact);

	/** shutdown projectile and prepare for destruction */
	void DisableAndDestroy();

	void SetGameplayEffectSpecs(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecs) { TargetGameplayEffectSpecs = GameplayEffectSpecs; }

private:
	UPROPERTY()
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	int ExplosionRadius;

	/** did it explode? */
	UPROPERTY()
	bool bExploded;
	
protected:
	/** Returns MovementComp subobject **/
	FORCEINLINE UProjectileMovementComponent* GetMovementComp() const { return ProjectileMovementComp; }
	/** Returns CollisionComp subobject **/
	FORCEINLINE USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ParticleComp subobject **/
	FORCEINLINE UParticleSystemComponent* GetParticleComp() const { return ParticleComp; }
};
