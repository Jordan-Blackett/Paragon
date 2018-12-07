// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParagonBasicAttack.h"
#include "GameFramework/DamageType.h"
#include "ParagonBasicAttack_Projectile.generated.h"

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

	/** damage at impact point */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	int32 ExplosionDamage;

	/** radius of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float ExplosionRadius;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	TSubclassOf<UDamageType> DamageType;

	/** defaults */
	FProjectileWeaponData()
	{
		ProjectileClass = NULL;
		ProjectileLife = 10.0f;
		ExplosionDamage = 100;
		ExplosionRadius = 300.0f;
		DamageType = UDamageType::StaticClass();
	}
};

/**
 * 
 */
UCLASS()
class PARAGON_API AParagonBasicAttack_Projectile : public AParagonBasicAttack
{
	GENERATED_BODY()

public:

	// Set weapon config
	void SetWeaponCongfig(FWeaponData WeaponData, FProjectileWeaponData ProjectileData);

	/** apply config on projectile */
	void ApplyWeaponConfig(FProjectileWeaponData& Data);

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() override;

protected:

	virtual EAmmoType GetAmmoType() const override
	{
		return EAmmoType::ERocket;
	}

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FProjectileWeaponData ProjectileConfig;

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

public:
	/** spawn projectile on server */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_FireProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir);
	void Server_FireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir);
	bool Server_FireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir);
};
