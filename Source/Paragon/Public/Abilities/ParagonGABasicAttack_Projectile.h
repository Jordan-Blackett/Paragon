// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/ParagonGABasicAttack.h"
#include "ParagonGABasicAttack_Projectile.generated.h"

USTRUCT()
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** Projectile class */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AParagonProjectile> ProjectileClass;

	/** Life time */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float ProjectileRange;

	/** Radius of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float ExplosionRadius;

	/** Defaults */
	FProjectileWeaponData()
	{
		ProjectileClass = NULL;
		ProjectileRange = 5.0f;
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
