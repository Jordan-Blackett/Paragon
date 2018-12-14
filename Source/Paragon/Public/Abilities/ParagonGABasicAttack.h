// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ParagonGABasicAttack.generated.h"

//class AShooterCharacter;
class UAnimMontage;
class UParticleSystemComponent;
//class UCameraShake;
//class UForceFeedbackEffect;
class UAudioComponent;
class USoundCue;

USTRUCT()
struct FWeaponEffectsData
{
	GENERATED_USTRUCT_BODY()

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MuzzleFX;

	/** camera shake on firing */
	//UPROPERTY(EditDefaultsOnly, Category = Effects)
	//TSubclassOf<UCameraShake> FireCameraShake;

	/** force feedback effect to play when the weapon is fired */
	//UPROPERTY(EditDefaultsOnly, Category = Effects)
	//UForceFeedbackEffect *FireForceFeedback;

	/** is muzzle FX looped? */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	uint32 bLoopedMuzzleFX : 1;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	FName MuzzleAttachPoint;

	FWeaponEffectsData()
	{
		MuzzleFX = nullptr;
		bLoopedMuzzleFX = 1;
		MuzzleAttachPoint = "";
	}
};

USTRUCT()
struct FWeaponSoundsData
{
	GENERATED_USTRUCT_BODY()

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireSound;

	/** looped fire sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireLoopSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireFinishSound;

	/** is fire sound looped? */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	uint32 bLoopedFireSound : 1;

	FWeaponSoundsData()
	{
		FireSound = nullptr;
		FireLoopSound = nullptr;
		FireFinishSound = nullptr;
		bLoopedFireSound = 1;
	}
};

USTRUCT()
struct FWeaponAnimationData
{
	GENERATED_USTRUCT_BODY()

	/** fire animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* FireAnim;

	/** is fire animation looped? */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	uint32 bLoopedFireAnim : 1;

	FWeaponAnimationData()
	{
		FireAnim = nullptr;
		bLoopedFireAnim = 1;
	}
};

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	TArray<TSubclassOf<UGameplayEffect>> HitGameplayEffects;

	/** is burst fire */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	uint32 bBurstFire : 1;

	/** number of shots in burst */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	int32 BurstShots;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float TimeBetweenShots;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	FName FirePointAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FWeaponEffectsData WeaponEffects;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	FWeaponSoundsData WeaponSounds;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnimationData WeaponAnimations;

	/** defaults */
	FWeaponData()
	{
		bBurstFire = 0;
		BurstShots = 0;
		TimeBetweenShots = 0.2f;
		FirePointAttachPoint = "";
	}
};

/**
 * 
 */
UCLASS()
class PARAGON_API UParagonGABasicAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** [local + server] firing started */
	void OnBurstStarted();

	/** [local + server] firing finished */
	//virtual void OnBurstFinished();

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(UParagonGABasicAttack::FireWeapon, );

	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** Called in network play to do the cosmetic fx for firing */
	void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	void StopSimulatingWeaponFire();

	////////////////////////////////////
	// --- Helper Functions ---

protected:

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponData WeaponConfig;

	////////////////////////////////////
	// --- Input - Server Side ---

	//UFUNCTION(reliable, server, WithValidation)
	//void Server_StartFire();
	//void Server_StartFire_Implementation();
	//bool Server_StartFire_Validate();

	////////////////////////////////////
	// --- Helper Functions ---

	/** spawned component for muzzle FX */
	UParticleSystemComponent* MuzzlePSC = nullptr;

	//UAudioComponent* FireAC = nullptr;

	///** is fire animation playing? */
	//uint32 bPlayingFireAnim : 1;

	///** is weapon fire active? */
	//uint32 bWantsToFire : 1;

	///** is equip animation playing? */
	//uint32 bPendingEquip : 1;

	///** weapon is refiring */
	//uint32 bRefiring;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;
	
};
