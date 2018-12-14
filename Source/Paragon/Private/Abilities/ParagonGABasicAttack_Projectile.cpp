// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonGABasicAttack_Projectile.h"
#include "ParagonCharacter.h"
#include "ParagonProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


void UParagonGABasicAttack_Projectile::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire Projectile"));
	FVector Origin;
	FVector ShootDir;
	FHitResult Impact = Cast<AParagonCharacter>(GetAvatarActorFromActorInfo())->LinetraceFromSocketOut(WeaponConfig.FirePointAttachPoint, 10000.0f, ShootDir, Origin);

	FTransform SpawnTM(FRotator::ZeroRotator, Origin); //ShootDir.Rotation()
	AParagonProjectile* Projectile = Cast<AParagonProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));
	if (Projectile)
	{
		Projectile->Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
		Projectile->SetOwner(GetAvatarActorFromActorInfo());
		Projectile->InitVelocity(ShootDir);

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}

	//Server_FireProjectile(Origin, ShootDir);
}

void UParagonGABasicAttack_Projectile::Server_FireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	FTransform SpawnTM(FRotator::ZeroRotator, Origin); //ShootDir.Rotation()
	AParagonProjectile* Projectile = Cast<AParagonProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));
	if (Projectile)
	{
		Projectile->Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
		Projectile->SetOwner(GetAvatarActorFromActorInfo());
		Projectile->InitVelocity(ShootDir);

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}

bool UParagonGABasicAttack_Projectile::Server_FireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}