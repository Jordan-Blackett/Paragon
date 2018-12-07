// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonBasicAttack_Projectile.h"
#include "ParagonProjectile.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AParagonBasicAttack_Projectile::FireWeapon()
{
	FVector ShootDir = GetAdjustedAim();
	FVector Origin = GetFirePointLocation();

	// Trace from camera to check what's under crosshair
	const float ProjectileAdjustRange = 10000.0f;
	//const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector StartTrace = Origin;
	const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true);

	// and adjust directions to hit that actor
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
		bool bWeaponPenetration = false;

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			// shooting backwards = weapon is penetrating
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			// check for weapon penetration if angle difference is big enough
			// raycast along weapon mesh to check if there's blocking hit

			FVector MuzzleStartTrace = Origin - GetFirePointDirection() * 150.0f;
			FVector MuzzleEndTrace = Origin;
			FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);

			if (MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}

		if (bWeaponPenetration)
		{
			// spawn at crosshair position
			Origin = Impact.ImpactPoint - ShootDir * 10.0f;
		}
		else
		{
			// adjust direction to hit
			ShootDir = AdjustedDir;
		}
	}

	Server_FireProjectile(Origin, ShootDir);
}

void AParagonBasicAttack_Projectile::Server_FireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	FTransform SpawnTM(ShootDir.Rotation(), Origin);
	AParagonProjectile* Projectile = Cast<AParagonProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));
	if (Projectile)
	{
		Projectile->Instigator = Instigator;
		Projectile->SetOwner(this);
		Projectile->InitVelocity(ShootDir);

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}

bool AParagonBasicAttack_Projectile::Server_FireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void AParagonBasicAttack_Projectile::ApplyWeaponConfig(FProjectileWeaponData& Data)
{
	Data = ProjectileConfig;
}

void AParagonBasicAttack_Projectile::SetWeaponCongfig(FWeaponData WeaponData, FProjectileWeaponData ProjectileData)
{
	WeaponConfig = WeaponData;
	ProjectileConfig = ProjectileData;
}