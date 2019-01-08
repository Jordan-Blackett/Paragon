// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/ParagonGABasicAttack.h"
#include "ParagonGABasicAttack_Projectile.generated.h"

USTRUCT()
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** projectile class */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AParagonProjectile> ProjectileClass;

	/** life time */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float ProjectileLife;

	/** radius of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float ExplosionRadius;

	/** defaults */
	FProjectileWeaponData()
	{
		ProjectileClass = NULL;
		//TargetGameplayEffectSpecs = NULL;
		ProjectileLife = 5.0f;
		ExplosionRadius = 100.0f;
	}
};

/**
 * 
 */
UCLASS()
class PARAGON_API UParagonGABasicAttack_Projectile : public UParagonGABasicAttack
{
	GENERATED_BODY()
	
public:

	/** Weapon specific fire implementation */
	virtual void FireWeapon() override;
	
protected:

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FProjectileWeaponData ProjectileConfig;
};
