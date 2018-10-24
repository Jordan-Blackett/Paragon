// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonBasicAttack_HitScan.h"
#include "ParagonCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "ParagonImpactEffect.h"

//AParagonBasicAttack_HitScan::AParagonBasicAttack_HitScan()
//{
//	CurrentFiringSpread = 0.0f;
//}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AParagonBasicAttack_HitScan::FireWeapon()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * HitScanConfig.WeaponRange;

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true);

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessHitScanHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);

	CurrentFiringSpread = FMath::Min(HitScanConfig.FiringSpreadMax, CurrentFiringSpread + HitScanConfig.FiringSpreadIncrement);
}

void AParagonBasicAttack_HitScan::Server_NotifyHit_Implementation(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	// if we have an instigator, calculate dot between the view and the shot
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetFirePointLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		DrawDebugLine(GetWorld(), Origin, ViewDir * HitScanConfig.WeaponRange, FColor::Red, true);

		// is the angle between the hit and the view within allowed limits (limit + weapon max angle)
		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > HitScanConfig.AllowedViewDotHitDir - WeaponAngleDot)
		{
			//if (CurrentState != EWeaponState::Idle)
			//{
			//	if (Impact.GetActor() == NULL)
			//	{
			//		if (Impact.bBlockingHit)
			//		{
			//			ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
			//		}
			//	}
			//	// assume it told the truth about static things because the don't move and the hit 
			//	// usually doesn't have significant gameplay implications
			//	else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
			//	{
			//		ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
			//	}
			//	else
			//	{
			//		// Get the component bounding box
			//		const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

			//		// calculate the box extent, and increase by a leeway
			//		FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
			//		BoxExtent *= InstantConfig.ClientSideHitLeeway;

			//		// avoid precision errors with really thin objects
			//		BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
			//		BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
			//		BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

			//		// Get the box center
			//		const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

			//		// if we are within client tolerance
			//		if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
			//			FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
			//			FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
			//		{
			//			ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
			//		}
			//		else
			//		{
			//			UE_LOG(LogShooterWeapon, Log, TEXT("%s Rejected client side hit of %s (outside bounding box tolerance)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
			//		}
			//	}
			//}
		}
		//else if (ViewDotHitDir <= InstantConfig.AllowedViewDotHitDir)
		//{
		//	UE_LOG(LogShooterWeapon, Log, TEXT("%s Rejected client side hit of %s (facing too far from the hit direction)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		//}
		//else
		//{
		//	UE_LOG(LogShooterWeapon, Log, TEXT("%s Rejected client side hit of %s"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		//}
	}
}

bool AParagonBasicAttack_HitScan::Server_NotifyHit_Validate(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	return true;
}

//void AParagonBasicAttack_HitScan::Server_NotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
//{
//	//const FVector Origin = GetMuzzleLocation();
//
//	//// play FX on remote clients
//	//HitNotify.Origin = Origin;
//	//HitNotify.RandomSeed = RandomSeed;
//	//HitNotify.ReticleSpread = ReticleSpread;
//
//	//// play FX locally
//	//if (GetNetMode() != NM_DedicatedServer)
//	//{
//	//	const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
//	//	SpawnTrailEffect(EndTrace);
//	//}
//}

//bool AParagonBasicAttack_HitScan::Server_NotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
//{
//	return true;
//}

void AParagonBasicAttack_HitScan::ProcessHitScanHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	if (MyPawn && MyPawn->GetIsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// if we're a client and we've hit something that is being controlled by the server
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// notify the server of the hit
			Server_NotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
		}
		else if (Impact.GetActor() == NULL)
		{
			if (Impact.bBlockingHit)
			{
				// notify the server of the hit
				Server_NotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
			}
			else
			{
				// notify server of the miss
				//ServerNotifyMiss(ShootDir, RandomSeed, ReticleSpread);
			}
		}
	}

	// process a confirmed hit
	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}

void AParagonBasicAttack_HitScan::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	// handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}

	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = Origin;
		HitNotify.RandomSeed = RandomSeed;
		HitNotify.ReticleSpread = ReticleSpread;
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir * HitScanConfig.WeaponRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		SpawnTrailEffect(EndPoint);
		SpawnImpactEffects(Impact);
	}
}

bool AParagonBasicAttack_HitScan::ShouldDealDamage(AActor* TestActor) const
{
	// if we're an actor on the server, or the actor's role is authoritative, we should register damage
	if (TestActor)
	{
		if (GetNetMode() != NM_Client ||
			TestActor->Role == ROLE_Authority ||
			TestActor->GetTearOff())
		{
			return true;
		}
	}

	return false;
}

void AParagonBasicAttack_HitScan::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = HitScanConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = HitScanConfig.HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}

void AParagonBasicAttack_HitScan::OnBurstFinished()
{
	Super::OnBurstFinished();

	CurrentFiringSpread = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

float AParagonBasicAttack_HitScan::GetCurrentSpread() const
{
	float FinalSpread = HitScanConfig.WeaponSpread + CurrentFiringSpread;
	//if (MyPawn && MyPawn->IsTargeting())
	//{
	//	FinalSpread *= InstantConfig.TargetingSpreadMod;
	//}

	return FinalSpread;
}


//////////////////////////////////////////////////////////////////////////
// Replication & effects

void AParagonBasicAttack_HitScan::OnRep_HitNotify()
{
	SimulateInstantHit(HitNotify.Origin, HitNotify.RandomSeed, HitNotify.ReticleSpread);
}

void AParagonBasicAttack_HitScan::SimulateInstantHit(const FVector& ShotOrigin, int32 RandomSeed, float ReticleSpread)
{
	FRandomStream WeaponRandomStream(RandomSeed);
	const float ConeHalfAngle = FMath::DegreesToRadians(ReticleSpread * 0.5f);

	const FVector StartTrace = ShotOrigin;
	const FVector AimDir = GetAdjustedAim();
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * HitScanConfig.WeaponRange;

	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	if (Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);
		SpawnTrailEffect(Impact.ImpactPoint);
	}
	else
	{
		SpawnTrailEffect(EndTrace);
	}
}

void AParagonBasicAttack_HitScan::SpawnImpactEffects(const FHitResult& Impact)
{
	if (HitScanConfig.ImpactTemplate && Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;

		// trace again to find component lost during replication
		if (!Impact.Component.IsValid())
		{
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			UseImpact = Hit;
		}

		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
		AParagonImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AParagonImpactEffect>(HitScanConfig.ImpactTemplate, SpawnTransform);
		if (EffectActor)
		{
			EffectActor->SurfaceHit = UseImpact;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}
}

void AParagonBasicAttack_HitScan::SpawnTrailEffect(const FVector& EndPoint)
{
	if (HitScanConfig.TrailFX)
	{
		const FVector Origin = GetFirePointLocation();
		//UE_LOG(LogTemp, Warning, TEXT("Trail"));
		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, HitScanConfig.TrailFX, Origin);
		if (TrailPSC)
		{
			TrailPSC->SetVectorParameter(HitScanConfig.TrailTargetParam, EndPoint);
		}
	}
}

void AParagonBasicAttack_HitScan::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(AParagonBasicAttack_HitScan, HitNotify, COND_SkipOwner);
}

void AParagonBasicAttack_HitScan::SetWeaponCongfig(FWeaponData WeaponData, FInstantWeaponData HitScanData)
{
	WeaponConfig = WeaponData;
	HitScanConfig = HitScanData;
}

