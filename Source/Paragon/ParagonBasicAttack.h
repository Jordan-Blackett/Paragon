// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ParagonBasicAttack.generated.h"

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
	};
}

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

	///** is muzzle FX looped? */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	uint32 bLoopedMuzzleFX : 1;

	FWeaponEffectsData()
	{
		MuzzleFX = nullptr;
		bLoopedMuzzleFX = 1;
	}
};

USTRUCT()
struct FWeaponSoundsData
{
	GENERATED_USTRUCT_BODY()

	///** single fire sound (bLoopedFireSound not set) */
	//UPROPERTY(EditDefaultsOnly, Category = Sound)
	//USoundCue* FireSound;

	///** looped fire sound (bLoopedFireSound set) */
	//UPROPERTY(EditDefaultsOnly, Category = Sound)
	//USoundCue* FireLoopSound;

	///** finished burst sound (bLoopedFireSound set) */
	//UPROPERTY(EditDefaultsOnly, Category = Sound)
	//USoundCue* FireFinishSound;

	///** out of ammo sound */
	//UPROPERTY(EditDefaultsOnly, Category = Sound)
	//USoundCue* OutOfAmmoSound;

	///** reload sound */
	//UPROPERTY(EditDefaultsOnly, Category = Sound)
	//USoundCue* ReloadSound;

	/** is fire sound looped? */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	uint32 bLoopedFireSound : 1;

	FWeaponSoundsData()
	{
		//FireSound = nullptr;
		//FireLoopSound = nullptr;
		//FireFinishSound = nullptr;
		//OutOfAmmoSound = nullptr;
		//ReloadSound = nullptr;
		bLoopedFireSound = 1;
	}
};

USTRUCT()
struct FWeaponAnimationData
{
	GENERATED_USTRUCT_BODY()

	///** fire animations */
	//UPROPERTY(EditDefaultsOnly, Category = Animation)
	//FWeaponAnim FireAnim;

	/** is fire animation looped? */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	uint32 bLoopedFireAnim : 1;

	FWeaponAnimationData()
	{
		bLoopedFireAnim = 1;
	}
};

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** inifite ammo for reloads */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	bool bInfiniteAmmo;

	/** infinite ammo in clip, no reload required */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	bool bInfiniteClip;

	/** max ammo */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 MaxAmmo;

	/** clip size */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 AmmoPerClip;

	/** initial clips */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 InitialClips;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float TimeBetweenShots;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	FName FirePointAttachPoint;

	/** firing audio (bLoopedFireSound set) */
	//UPROPERTY(Transient)
	//	UAudioComponent* FireAC;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FWeaponEffectsData WeaponEffects;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	FWeaponSoundsData WeaponSounds;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnimationData WeaponAnimations;

	/** defaults */
	FWeaponData()
	{
		bInfiniteAmmo = false;
		bInfiniteClip = false;
		MaxAmmo = 100;
		AmmoPerClip = 20;
		InitialClips = 4;
		TimeBetweenShots = 0.2f;
		FirePointAttachPoint = "";
	}
};

class AParagonCharacter;
class UParticleSystemComponent;

UCLASS()
class PARAGON_API AParagonBasicAttack : public AInfo
{
	GENERATED_BODY()

public:
	//////////////////////////////////////////////////////////////////////////
	// Ammo

	enum class EAmmoType
	{
		EBullet,
		ERocket,
		EMax,
	};

	/** [server] add ammo */
	void GiveAmmo(int AddAmount);

	/** consume a bullet */
	void UseAmmo();

	/** query ammo type */
	virtual EAmmoType GetAmmoType() const
	{
		return EAmmoType::EBullet;
	}

	//////////////////////////////////////////////////////////////////////////
	// Input

	/** [local + server] start weapon fire */
	virtual void StartFire();

	/** [local + server] stop weapon fire */
	virtual void StopFire();

	/** [all] start weapon reload */
	virtual void StartReload(bool bFromReplication = false);

	/** [local + server] interrupt weapon reload */
	virtual void StopReload();

	/** [server] performs actual reload */
	virtual void ReloadWeapon();

	/** trigger reload from server */
	UFUNCTION(reliable, client)
	void Client_StartReload();
	void Client_StartReload_Implementation();

	//////////////////////////////////////////////////////////////////////////
	// Control

	/** check if weapon can fire */
	bool CanFire() const;

	/** check if weapon can be reloaded */
	bool CanReload() const;

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	/** get current weapon state */
	EWeaponState::Type GetCurrentState() const;

	/** get current ammo amount (total) */
	int32 GetCurrentAmmo() const;

	/** get current ammo amount (clip) */
	int32 GetCurrentAmmoInClip() const;

	/** get clip size */
	int32 GetAmmoPerClip() const;

	/** get max ammo amount */
	int32 GetMaxAmmo() const;

	/** get weapon mesh (needs pawn owner to determine variant) */
	//USkeletalMeshComponent* GetWeaponMesh() const;

	/** get pawn owner */
	//UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	//class AShooterCharacter* GetPawnOwner() const;

	///** icon displayed on the HUD when weapon is equipped as primary */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//FCanvasIcon PrimaryIcon;

	///** icon displayed on the HUD when weapon is secondary */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//FCanvasIcon SecondaryIcon;

	///** bullet icon used to draw current clip (left side) */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//FCanvasIcon PrimaryClipIcon;

	///** bullet icon used to draw secondary clip (left side) */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//FCanvasIcon SecondaryClipIcon;

	///** how many icons to draw per clip */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//float AmmoIconsCount;

	///** defines spacing between primary ammo icons (left side) */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//int32 PrimaryClipIconOffset;

	///** defines spacing between secondary ammo icons (left side) */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//int32 SecondaryClipIconOffset;

	///** crosshair parts icons (left, top, right, bottom and center) */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//FCanvasIcon Crosshair[5];

	///** crosshair parts icons when targeting (left, top, right, bottom and center) */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//FCanvasIcon AimingCrosshair[5];

	///** only use red colored center part of aiming crosshair */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//bool UseLaserDot;

	///** false = default crosshair */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//bool UseCustomCrosshair;

	///** false = use custom one if set, otherwise default crosshair */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//bool UseCustomAimingCrosshair;

	///** true - crosshair will not be shown unless aiming with the weapon */
	//UPROPERTY(EditDefaultsOnly, Category = HUD)
	//bool bHideCrosshairWhileNotAiming;

	/** check if weapon has infinite ammo (include owner's cheats) */
	bool HasInfiniteAmmo() const;

	/** check if weapon has infinite clip (include owner's cheats) */
	bool HasInfiniteClip() const;

	///** set the weapon's owning pawn */
	//void SetOwningPawn(AShooterCharacter* AShooterCharacter);

public:	
	// Sets default values for this actor's properties
	AParagonBasicAttack();

	void InitBasicAttack(AParagonCharacter* NewOwner);

protected:
	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponData WeaponConfig;

	UPROPERTY()
	class AParagonCharacter* MyPawn = nullptr;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

private:
	// Character mesh
	//UPROPERTY()
	//USkeletalMeshComponent* CharacterMesh = nullptr;

	//UPROPERTY()
	//class AParagonCharacter* PlayerController;

	//UPROPERTY()
	//UWorld* SceneWorld = nullptr;

protected:

	//////////////////////////////////////////////////////////////////////////
	// Input - server side

	UFUNCTION(reliable, server, WithValidation)
	void Server_StartFire();
	void Server_StartFire_Implementation();
	bool Server_StartFire_Validate();

	UFUNCTION(reliable, server, WithValidation)
	void Server_StopFire();
	void Server_StopFire_Implementation();
	bool Server_StopFire_Validate();

	UFUNCTION(reliable, server, WithValidation)
	void Server_StartReload();
	void Server_StartReload_Implementation();
	bool Server_StartReload_Validate();

	UFUNCTION(reliable, server, WithValidation)
	void Server_StopReload();
	void Server_StopReload_Implementation();
	bool Server_StopReload_Validate();

	//////////////////////////////////////////////////////////////////////////
	// Replication & effects

	UFUNCTION()
	void OnRep_BurstCounter();

	UFUNCTION()
	void OnRep_Reload();

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(AShooterWeapon::FireWeapon, );

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
	void Server_HandleFiring();
	void Server_HandleFiring_Implementation();
	bool Server_HandleFiring_Validate();

	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** [local + server] firing started */
	virtual void OnBurstStarted();

	/** [local + server] firing finished */
	virtual void OnBurstFinished();

	/** update weapon state */
	void SetWeaponState(EWeaponState::Type NewState);

	/** determine current weapon state */
	void DetermineWeaponState();

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage helpers

	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim() const;

	/** Get the aim of the camera */
	FVector GetCameraAim() const;

	/** get the originating location for camera damage */
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	/** get the muzzle location of the weapon */
	FVector GetFirePointLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetFirePointDirection() const;

	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	//////////////////////////////////////////////////////////////////////////
	//

	/** spawned component for muzzle FX */
	UParticleSystemComponent* MuzzlePSC = nullptr;

	/** is fire animation playing? */
	uint32 bPlayingFireAnim : 1;

	/** is weapon fire active? */
	uint32 bWantsToFire : 1;

	/** is reload animation playing? */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Reload)
	uint32 bPendingReload : 1;

	/** is equip animation playing? */
	uint32 bPendingEquip : 1;

	/** weapon is refiring */
	uint32 bRefiring;

	/** current weapon state */
	EWeaponState::Type CurrentState;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** current total ammo */
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;

	/** current ammo - inside clip */
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmoInClip;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;

	/** Handle for efficient management of StopReload timer */
	FTimerHandle TimerHandle_StopReload;

	/** Handle for efficient management of ReloadWeapon timer */
	FTimerHandle TimerHandle_ReloadWeapon;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;

};
