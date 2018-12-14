// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonGABasicAttack.h"
#include "Net/UnrealNetwork.h"
//#include "ParagonCharacter.h"
#include "Kismet/GameplayStatics.h"
//#include "GameFramework/Actor.h"
//#include "TimerManager.h"
//#include "Engine/World.h"



void UParagonGABasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("SERVER"));

	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		HandleFiring();
	}
	//EndAbility(Handle);
}

void UParagonGABasicAttack::OnBurstStarted()
{
	// Start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
		LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		//GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &UParagonGABasicAttack::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void UParagonGABasicAttack::HandleFiring()
{
	// local client will notify server
	//if (GetAvatarActorFromActorInfo()->GetNetMode() < ROLE_Authority)
	//{
	//	//Server_HandleFiring();
	//}

	//SimulateWeaponFire();




	FireWeapon();



	//if (MyPawn && MyPawn->IsLocallyControlled())
	//{

	//	// update firing FX on remote clients if function was called on server
	//	BurstCounter++;

	//else if (MyPawn && MyPawn->IsLocallyControlled())
	//{


	//	// stop weapon fire FX, but stay in Firing state
	//	if (BurstCounter > 0)
	//	{
	//		OnBurstFinished();
	//	

	//if (MyPawn && MyPawn->IsLocallyControlled())
	//{
	//	// local client will notify server

	//	bool burstFire = false;
	//	if (WeaponConfig.bBurstFire && BurstCounter >= WeaponConfig.BurstShots)
	//	{
	//		burstFire = true;
	//		StopFire();
	//	}

	//	// setup refire timer
	//	bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
	//	if (bRefiring && !burstFire)
	//	{
	//		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AParagonBasicAttack::HandleFiring, WeaponConfig.TimeBetweenShots, false);
	//	}
	//}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void UParagonGABasicAttack::SimulateWeaponFire()
{
	//if (Role == ROLE_Authority)
	//{
	//	return;
	//}

	if (WeaponConfig.WeaponEffects.MuzzleFX)
	{
		USkeletalMeshComponent* CharacterMesh = GetOwningComponentFromActorInfo();

		if (!WeaponConfig.WeaponEffects.bLoopedMuzzleFX || MuzzlePSC == nullptr)
		{
			if (CharacterMesh != nullptr) 
			{
				if (WeaponConfig.FirePointAttachPoint != "")
				{
					MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(WeaponConfig.WeaponEffects.MuzzleFX, CharacterMesh, WeaponConfig.WeaponEffects.MuzzleAttachPoint);
				}
			}
		}
	}

	//if (!WeaponConfig.WeaponAnimations.bLoopedFireAnim || !bPlayingFireAnim)
	//{
	//	PlayWeaponAnimation(WeaponConfig.WeaponAnimations.FireAnim);
	//	bPlayingFireAnim = true;
	//}

		////UE_LOG(LogTemp, Warning, TEXT("sound2"));
		//if (WeaponConfig.WeaponSounds.bLoopedFireSound)
		//{
		//	if (FireAC == NULL)
		//	{
		//		FireAC = PlayWeaponSound(WeaponConfig.WeaponSounds.FireSound); //FireLoopSound
		//	}
		//}
		//else
		//{
		//	PlayWeaponSound(WeaponConfig.WeaponSounds.FireSound);
		//}

		////AShooterPlayerController* PC = (MyPawn != NULL) ? Cast<AShooterPlayerController>(MyPawn->Controller) : NULL;
		////if (PC != NULL && PC->IsLocalController())
		////{
		////	if (FireCameraShake != NULL)
		////	{
		////		PC->ClientPlayCameraShake(FireCameraShake, 1);
		////	}
		////	if (FireForceFeedback != NULL && PC->IsVibrationEnabled())
		////	{
		////		PC->ClientPlayForceFeedback(FireForceFeedback, false, false, "Weapon");
		////	}
		////}
}



