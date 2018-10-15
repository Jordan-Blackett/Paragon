// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonProjectile.h"
#include "Paragon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"


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
	bReplicateMovement = true;
}

void AParagonProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ProjectileMovementComp->OnProjectileStop.AddDynamic(this, &AParagonProjectile::OnImpact);
	CollisionComp->MoveIgnoreActors.Add(GetOwner()); //Instigator

	AParagonBasicAttack_Projectile* OwnerWeapon = Cast<AParagonBasicAttack_Projectile>(GetOwner());
	if (OwnerWeapon)
	{
		OwnerWeapon->ApplyWeaponConfig(WeaponConfig);
	}

	SetLifeSpan(WeaponConfig.ProjectileLife);
	MyController = GetInstigatorController();
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
		Explode(HitResult);
		DisableAndDestroy();
	}
}

void AParagonProjectile::Explode(const FHitResult& Impact)
{
	if (ParticleComp)
	{
		ParticleComp->Deactivate();
	}

	// effects and damage origin shouldn't be placed inside mesh at impact point
	const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;

	if (WeaponConfig.ExplosionDamage > 0 && WeaponConfig.ExplosionRadius > 0 && WeaponConfig.DamageType)
	{
		//UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, NudgedImpactLocation, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get());
	}

	//if (ExplosionTemplate)
	//{
	//	FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), NudgedImpactLocation);
	//	//AShooterExplosionEffect* const EffectActor = GetWorld()->SpawnActorDeferred<AShooterExplosionEffect>(ExplosionTemplate, SpawnTransform);
	//	//if (EffectActor)
	//	//{
	//	//	EffectActor->SurfaceHit = Impact;
	//	//	UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
	//	//}
	//}

	bExploded = true;
}

void AParagonProjectile::DisableAndDestroy()
{
	//UAudioComponent* ProjAudioComp = FindComponentByClass<UAudioComponent>();
	//if (ProjAudioComp && ProjAudioComp->IsPlaying())
	//{
	//	ProjAudioComp->FadeOut(0.1f, 0.f);
	//}

	//MovementComp->StopMovementImmediately();

	//// give clients some time to show explosion
	//SetLifeSpan(2.0f);
}

///CODE_SNIPPET_START: AActor::GetActorLocation AActor::GetActorRotation
void AParagonProjectile::OnRep_Exploded()
{
	//FVector ProjDirection = GetActorForwardVector();

	//const FVector StartTrace = GetActorLocation() - ProjDirection * 200;
	//const FVector EndTrace = GetActorLocation() + ProjDirection * 150;
	FHitResult Impact;

	//if (!GetWorld()->LineTraceSingleByChannel(Impact, StartTrace, EndTrace, COLLISION_PROJECTILE, FCollisionQueryParams(SCENE_QUERY_STAT(ProjClient), true, Instigator)))
	//{
	//	// failsafe
	//	Impact.ImpactPoint = GetActorLocation();
	//	Impact.ImpactNormal = -ProjDirection;
	//}

	Explode(Impact);
}
///CODE_SNIPPET_END

void AParagonProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->Velocity = NewVelocity;
	}
}

void AParagonProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AParagonProjectile, bExploded);
}