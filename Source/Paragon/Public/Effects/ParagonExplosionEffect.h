// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParagonExplosionEffect.generated.h"

//
// Spawnable effect for explosion - NOT replicated to clients
// Each explosion type should be defined as separate blueprint
//
UCLASS()
class PARAGON_API AParagonExplosionEffect : public AActor
{
	GENERATED_BODY()

	/** explosion FX */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	UParticleSystem* ExplosionFX;
	
public:	
	// Sets default values for this actor's properties
	AParagonExplosionEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	/** how long keep explosion light on? */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	float ExplosionLightFadeOut;

	/** explosion sound */
	//UPROPERTY(EditDefaultsOnly, Category = Effect)
	//USoundCue* ExplosionSound;

	/** explosion decals */
	//UPROPERTY(EditDefaultsOnly, Category = Effect)
	//struct FDecalData Decal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = Surface)
	FHitResult SurfaceHit;

	/** update fading light */
	virtual void Tick(float DeltaSeconds) override;

private:
	/** explosion light */
	//UPROPERTY(VisibleDefaultsOnly, Category = Effect)
	//UPointLightComponent* ExplosionLight;

	/** Point light component name */
	FName ExplosionLightComponentName;

public:
	/** Returns ExplosionLight subobject **/
	//FORCEINLINE UPointLightComponent* GetExplosionLight() const { return ExplosionLight; }
	
};
