// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonBasicAttack.h"
#include "Paragon.h"
#include "ParagonCharacter.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AParagonBasicAttack::AParagonBasicAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	BurstCounter = 0;
	LastFireTime = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	//Instigator = GetOwner();
}

void AParagonBasicAttack::InitBasicAttack(AParagonCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// Net owner for RPC calls
		SetOwner(NewOwner);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AParagonBasicAttack::StartFire()
{
	//if (Role < ROLE_Authority)
	//{
	//	Server_StartFire();
	//}

	//if (!bWantsToFire)
	//{
	//	bWantsToFire = true;
	//	DetermineWeaponState();
	//}
}

void AParagonBasicAttack::StopFire()
{
	//if (Role < ROLE_Authority)
	//{
	//	Server_StopFire();
	//}

	//if (bWantsToFire)
	//{
	//	bWantsToFire = false;
	//	DetermineWeaponState();
	//}
}

void AParagonBasicAttack::StartReload(bool bFromReplication)
{
	//if (!bFromReplication && Role < ROLE_Authority)
	//{
	//	Server_StartReload();
	//}

	//if (bFromReplication || CanReload())
	//{
	//	bPendingReload = true;
	//	DetermineWeaponState();

	//	float AnimDuration = PlayWeaponAnimation(ReloadAnim);
	//	if (AnimDuration <= 0.0f)
	//	{
	//		AnimDuration = WeaponConfig.NoAnimReloadDuration;
	//	}

	//	GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &AShooterWeapon::StopReload, AnimDuration, false);
	//	if (Role == ROLE_Authority)
	//	{
	//		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AShooterWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
	//	}

	//	if (MyPawn && MyPawn->IsLocallyControlled())
	//	{
	//		PlayWeaponSound(ReloadSound);
	//	}
	//}
}

void AParagonBasicAttack::StopReload()
{
	//if (CurrentState == EWeaponState::Reloading)
	//{
	//	bPendingReload = false;
	//	DetermineWeaponState();
	//	StopWeaponAnimation(ReloadAnim);
	//}
}

void AParagonBasicAttack::Server_StartFire_Implementation()
{
	//StartFire();
}

bool AParagonBasicAttack::Server_StartFire_Validate()
{
	return true;
}

void AParagonBasicAttack::Server_StopFire_Implementation()
{
	//StopFire();
}

bool AParagonBasicAttack::Server_StopFire_Validate()
{
	return true;
}

void AParagonBasicAttack::Server_StartReload_Implementation()
{
	//StartReload();
}

bool AParagonBasicAttack::Server_StartReload_Validate()
{
	return true;
}

void AParagonBasicAttack::Server_StopReload_Implementation()
{
	//StopReload();
}

bool AParagonBasicAttack::Server_StopReload_Validate()
{
	return true;
}

void AParagonBasicAttack::Client_StartReload_Implementation()
{
	//StartReload();
}

//////////////////////////////////////////////////////////////////////////
// Replication & effects

void AParagonBasicAttack::OnRep_BurstCounter()
{
	//if (BurstCounter > 0)
	//{
	//	SimulateWeaponFire();
	//}
	//else
	//{
	//	StopSimulatingWeaponFire();
	//}
}

void AParagonBasicAttack::OnRep_Reload()
{
	//if (bPendingReload)
	//{
	//	StartReload(true);
	//}
	//else
	//{
	//	StopReload();
	//}
}

void AParagonBasicAttack::SimulateWeaponFire()
{
	//if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing)
	//{
	//	return;
	//}

	//if (MuzzleFX)
	//{
	//	USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
	//	if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
	//	{
	//		// Split screen requires we create 2 effects. One that we see and one that the other player sees.
	//		if ((MyPawn != NULL) && (MyPawn->IsLocallyControlled() == true))
	//		{
	//			AController* PlayerCon = MyPawn->GetController();
	//			if (PlayerCon != NULL)
	//			{
	//				Mesh1P->GetSocketLocation(MuzzleAttachPoint);
	//				MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh1P, MuzzleAttachPoint);
	//				MuzzlePSC->bOwnerNoSee = false;
	//				MuzzlePSC->bOnlyOwnerSee = true;

	//				Mesh3P->GetSocketLocation(MuzzleAttachPoint);
	//				MuzzlePSCSecondary = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh3P, MuzzleAttachPoint);
	//				MuzzlePSCSecondary->bOwnerNoSee = true;
	//				MuzzlePSCSecondary->bOnlyOwnerSee = false;
	//			}
	//		}
	//		else
	//		{
	//			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, UseWeaponMesh, MuzzleAttachPoint);
	//		}
	//	}
	//}

	//if (!bLoopedFireAnim || !bPlayingFireAnim)
	//{
	//	PlayWeaponAnimation(FireAnim);
	//	bPlayingFireAnim = true;
	//}

	//if (bLoopedFireSound)
	//{
	//	if (FireAC == NULL)
	//	{
	//		FireAC = PlayWeaponSound(FireLoopSound);
	//	}
	//}
	//else
	//{
	//	PlayWeaponSound(FireSound);
	//}

	//AShooterPlayerController* PC = (MyPawn != NULL) ? Cast<AShooterPlayerController>(MyPawn->Controller) : NULL;
	//if (PC != NULL && PC->IsLocalController())
	//{
	//	if (FireCameraShake != NULL)
	//	{
	//		PC->ClientPlayCameraShake(FireCameraShake, 1);
	//	}
	//	if (FireForceFeedback != NULL && PC->IsVibrationEnabled())
	//	{
	//		PC->ClientPlayForceFeedback(FireForceFeedback, false, false, "Weapon");
	//	}
	//}
}

void AParagonBasicAttack::StopSimulatingWeaponFire()
{
	//if (bLoopedMuzzleFX)
	//{
	//	if (MuzzlePSC != NULL)
	//	{
	//		MuzzlePSC->DeactivateSystem();
	//		MuzzlePSC = NULL;
	//	}
	//	if (MuzzlePSCSecondary != NULL)
	//	{
	//		MuzzlePSCSecondary->DeactivateSystem();
	//		MuzzlePSCSecondary = NULL;
	//	}
	//}

	//if (bLoopedFireAnim && bPlayingFireAnim)
	//{
	//	StopWeaponAnimation(FireAnim);
	//	bPlayingFireAnim = false;
	//}

	//if (FireAC)
	//{
	//	FireAC->FadeOut(0.1f, 0.0f);
	//	FireAC = NULL;

	//	PlayWeaponSound(FireFinishSound);
	//}
}

void AParagonBasicAttack::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AShooterWeapon, MyPawn);

	//DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmo, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmoInClip, COND_OwnerOnly);

	//DOREPLIFETIME_CONDITION(AShooterWeapon, BurstCounter, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AShooterWeapon, bPendingReload, COND_SkipOwner);
}

//////////////////////////////////////////////////////////////////////////
// Control

bool AParagonBasicAttack::CanFire() const
{
	//bool bCanFire = MyPawn && MyPawn->CanFire();
	//bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	//return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
	return true;
}

bool AParagonBasicAttack::CanReload() const
{
	//bool bCanReload = (!MyPawn || MyPawn->CanReload());
	//bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0 || HasInfiniteClip());
	//bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	//return ((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AParagonBasicAttack::GiveAmmo(int AddAmount)
{
	//const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
	//AddAmount = FMath::Min(AddAmount, MissingAmmo);
	//CurrentAmmo += AddAmount;

	//AShooterAIController* BotAI = MyPawn ? Cast<AShooterAIController>(MyPawn->GetController()) : NULL;
	//if (BotAI)
	//{
	//	BotAI->CheckAmmo(this);
	//}

	//// start reload if clip was empty
	//if (GetCurrentAmmoInClip() <= 0 &&
	//	CanReload() &&
	//	MyPawn && (MyPawn->GetWeapon() == this))
	//{
	//	ClientStartReload();
	//}
}

void AParagonBasicAttack::UseAmmo()
{
	/*if (!HasInfiniteAmmo())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo() && !HasInfiniteClip())
	{
		CurrentAmmo--;
	}

	AShooterAIController* BotAI = MyPawn ? Cast<AShooterAIController>(MyPawn->GetController()) : NULL;
	AShooterPlayerController* PlayerController = MyPawn ? Cast<AShooterPlayerController>(MyPawn->GetController()) : NULL;
	if (BotAI)
	{
		BotAI->CheckAmmo(this);
	}
	else if (PlayerController)
	{
		AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>(PlayerController->PlayerState);
		switch (GetAmmoType())
		{
		case EAmmoType::ERocket:
			PlayerState->AddRocketsFired(1);
			break;
		case EAmmoType::EBullet:
		default:
			PlayerState->AddBulletsFired(1);
			break;
		}
	}*/
}

void AParagonBasicAttack::HandleFiring()
{
	//if ((CurrentAmmoInClip > 0 || HasInfiniteClip() || HasInfiniteAmmo()) && CanFire())
	//{
	//	if (GetNetMode() != NM_DedicatedServer)
	//	{
	//		SimulateWeaponFire();
	//	}

	//	if (MyPawn && MyPawn->IsLocallyControlled())
	//	{
	//		FireWeapon();

	//		UseAmmo();

	//		// update firing FX on remote clients if function was called on server
	//		BurstCounter++;
	//	}
	//}
	//else if (CanReload())
	//{
	//	StartReload();
	//}
	//else if (MyPawn && MyPawn->IsLocallyControlled())
	//{
	//	if (GetCurrentAmmo() == 0 && !bRefiring)
	//	{
	//		PlayWeaponSound(OutOfAmmoSound);
	//		AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(MyPawn->Controller);
	//		AShooterHUD* MyHUD = MyPC ? Cast<AShooterHUD>(MyPC->GetHUD()) : NULL;
	//		if (MyHUD)
	//		{
	//			MyHUD->NotifyOutOfAmmo();
	//		}
	//	}

	//	// stop weapon fire FX, but stay in Firing state
	//	if (BurstCounter > 0)
	//	{
	//		OnBurstFinished();
	//	}
	//}

	//if (MyPawn && MyPawn->IsLocallyControlled())
	//{
	//	// local client will notify server
	//	if (Role < ROLE_Authority)
	//	{
	//		ServerHandleFiring();
	//	}

	//	// reload after firing last round
	////	if (CurrentAmmoInClip <= 0 && CanReload())
	////	{
	////		StartReload();
	////	}

	////	// setup refire timer
	////	bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
	////	if (bRefiring)
	////	{
	////		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AShooterWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
	////	}
	////}

	////LastFireTime = GetWorld()->GetTimeSeconds();
}

void AParagonBasicAttack::Server_HandleFiring_Implementation()
{
	//const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	//HandleFiring();

	//if (bShouldUpdateAmmo)
	//{
	//	// update ammo
	//	UseAmmo();

	//	// update firing FX on remote clients
	//	BurstCounter++;
	//}
}

bool AParagonBasicAttack::Server_HandleFiring_Validate()
{
	return true;
}

void AParagonBasicAttack::ReloadWeapon()
{
	//int32 ClipDelta = FMath::Min(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	//if (HasInfiniteClip())
	//{
	//	ClipDelta = WeaponConfig.AmmoPerClip - CurrentAmmoInClip;
	//}

	//if (ClipDelta > 0)
	//{
	//	CurrentAmmoInClip += ClipDelta;
	//}

	//if (HasInfiniteClip())
	//{
	//	CurrentAmmo = FMath::Max(CurrentAmmoInClip, CurrentAmmo);
	//}
}

//void AParagonBasicAttack::SetWeaponState(EWeaponState::Type NewState)
//{
//	//const EWeaponState::Type PrevState = CurrentState;
//
//	//if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
//	//{
//	//	OnBurstFinished();
//	//}
//
//	//CurrentState = NewState;
//
//	//if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
//	//{
//	//	OnBurstStarted();
//	//}
//}

void AParagonBasicAttack::DetermineWeaponState()
{
	//EWeaponState::Type NewState = EWeaponState::Idle;

	//if (bIsEquipped)
	//{
	//	if (bPendingReload)
	//	{
	//		if (CanReload() == false)
	//		{
	//			NewState = CurrentState;
	//		}
	//		else
	//		{
	//			NewState = EWeaponState::Reloading;
	//		}
	//	}
	//	else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
	//	{
	//		NewState = EWeaponState::Firing;
	//	}
	//}
	//else if (bPendingEquip)
	//{
	//	NewState = EWeaponState::Equipping;
	//}

	//SetWeaponState(NewState);
}

void AParagonBasicAttack::OnBurstStarted()
{
	//// start firing, can be delayed to satisfy TimeBetweenShots
	//const float GameTime = GetWorld()->GetTimeSeconds();
	//if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
	//	LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	//{
	//	GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AShooterWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	//}
	//else
	//{
	//	HandleFiring();
	//}
}

void AParagonBasicAttack::OnBurstFinished()
{
	//// stop firing FX on remote clients
	//BurstCounter = 0;

	//// stop firing FX locally, unless it's a dedicated server
	//if (GetNetMode() != NM_DedicatedServer)
	//{
	//	StopSimulatingWeaponFire();
	//}

	//GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	//bRefiring = false;
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

FVector AParagonBasicAttack::GetAdjustedAim() const
{
	AParagonCharacter* const PlayerController = Instigator ? Cast<AParagonCharacter>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;
	
	// If we have a player controller use it for the aim
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		// Now see if we have an AI controller - we will want to get the aim from there if we do
		//AShooterAIController* AIController = MyPawn ? Cast<AShooterAIController>(MyPawn->Controller) : NULL;
		//if (AIController != NULL)
		//{
		//	FinalAim = AIController->GetControlRotation().Vector();
		//}
		//else
		//{
			FinalAim = Instigator->GetBaseAimRotation().Vector();
		//}
	}
	return FinalAim;
}

FVector AParagonBasicAttack::GetCameraAim() const
{
	AParagonCharacter* const PlayerController = Instigator ? Cast<AParagonCharacter>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;

	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		FinalAim = Instigator->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}

FVector AParagonBasicAttack::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	AParagonCharacter* PC = MyPawn ? Cast<AParagonCharacter>(MyPawn->Controller) : NULL;
	//AShooterAIController* AIPC = MyPawn ? Cast<AShooterAIController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	}
	//else if (AIPC)
	//{
		//OutStartTrace = GetMuzzleLocation();
	//}

	return OutStartTrace;
}

FVector AParagonBasicAttack::GetFirePointLocation() const
{
	AParagonCharacter* PlayerCharacter = Cast<AParagonCharacter>(GetOwner());
	if (PlayerCharacter != nullptr)
	{
		return PlayerCharacter->GetMesh()->GetSocketLocation(WeaponConfig.FirePointAttachPoint);
	}
	else
	{
		return FVector::ZeroVector;
	}
}

FVector AParagonBasicAttack::GetFirePointDirection() const
{
	AParagonCharacter* PlayerCharacter = Cast<AParagonCharacter>(GetOwner());
	if (PlayerCharacter != nullptr)
	{
		return PlayerCharacter->GetMesh()->GetSocketRotation(WeaponConfig.FirePointAttachPoint).Vector();
	}
	else
	{
		return FVector::ZeroVector;
	}
}

FHitResult AParagonBasicAttack::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}