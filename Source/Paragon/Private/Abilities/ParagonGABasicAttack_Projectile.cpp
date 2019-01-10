// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonGABasicAttack_Projectile.h"
#include "ParagonCharacter.h"
#include "ParagonProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


void UParagonGABasicAttack_Projectile::FireWeapon()
{
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
		Projectile->Init(ProjectileConfig.ProjectileLife, ProjectileConfig.ExplosionRadius);

		if(WeaponConfig.HitGameplayEffects.Num() > 0)
		{
			TArray<FGameplayEffectSpecHandle> SpecHandle;
			SpecHandle.Add(MakeOutgoingGameplayEffectSpec(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), WeaponConfig.HitGameplayEffects[0], GetAbilityLevel()));
			Projectile->SetGameplayEffectSpecs(SpecHandle);
		}

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}