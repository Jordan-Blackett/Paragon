// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonProjectile.h"
#include "Paragon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ParagonExplosionEffect.h"
#include "ParagonCharacter.h"
#include "DrawDebugHelpers.h"
#include "ParagonDebugConsoleVariables.h"

extern TAutoConsoleVariable<int32> CVarDebugBasicAttack;

// Sets default values
AParagonProjectile::AParagonProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RootComponent = CollisionComp;
	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = false;
	ParticleComp->bAutoDestroy = false;
	ParticleComp->SetupAttachment(RootComponent);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovementComp->UpdatedComponent = CollisionComp;
	ProjectileMovementComp->InitialSpeed = 2000.0f;
	ProjectileMovementComp->MaxSpeed = 2000.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
}

void AParagonProjectile::Init(float Lifetime, float NewExplosionRadius)
{
	SetLifeSpan(Lifetime);
	ExplosionRadius = NewExplosionRadius;
}

void AParagonProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ProjectileMovementComp->OnProjectileStop.AddDynamic(this, &AParagonProjectile::OnImpact);
	CollisionComp->MoveIgnoreActors.Add(Cast<AActor>(Instigator));

	//AParagonBasicAttack_Projectile* OwnerWeapon = Cast<AParagonBasicAttack_Projectile>(GetOwner());
	//if (OwnerWeapon)
	//{
	//	OwnerWeapon->ApplyWeaponConfig(WeaponConfig);
	//}

	//SetLifeSpan(WeaponConfig.ProjectileLife);
	//MyController = GetInstigatorController();
}

void AParagonProjectile::InitVelocity(FVector& ShootDirection)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->Velocity = ShootDirection * ProjectileMovementComp->InitialSpeed;
	}
}

void AParagonProjectile::OnImpact(const FHitResult& HitResult)
{
	if (Role == ROLE_Authority && !bExploded)
	{
		// Deal Damage
		TArray<TEnumAsByte<EObjectTypeQuery>> CollisionType;
		CollisionType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TSubclassOf<AParagonCharacter> ActorFilter;
		TArray<AActor*> ActorsIgnore;
		ActorsIgnore.Add(Instigator);
		TArray<AActor*> OverlappedActors;

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), HitResult.Location, ExplosionRadius, CollisionType, ActorFilter, ActorsIgnore, OverlappedActors);

		int32 Debug = CVarDebugBasicAttack.GetValueOnGameThread();
		if (Debug)
		{
			DrawDebugSphere(GetWorld(), HitResult.Location, ExplosionRadius, 24, FColor::Green, true, 4.0f);
		}

		for (AActor* Actor : OverlappedActors)
		{
			Cast<AParagonCharacter>(Actor)->TakeDamageEffectSpecs(TargetGameplayEffectSpecs);
		}
	}

	Explode(HitResult);
	DisableAndDestroy();
}

void AParagonProjectile::Explode(const FHitResult& Impact)
{
	if (ParticleComp)
	{
		ParticleComp->Deactivate();
	}

	// effects and damage origin shouldn't be placed inside mesh at impact point
	const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;

	if (ExplosionTemplate)
	{
		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), NudgedImpactLocation);
		AParagonExplosionEffect* const EffectActor = GetWorld()->SpawnActorDeferred<AParagonExplosionEffect>(ExplosionTemplate, SpawnTransform);
		if (EffectActor)
		{
			EffectActor->SurfaceHit = Impact;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}

	bExploded = true;
}

void AParagonProjectile::DisableAndDestroy()
{
	ProjectileMovementComp->StopMovementImmediately();

	// give clients some time to show explosion
	SetLifeSpan(2.0f);
}