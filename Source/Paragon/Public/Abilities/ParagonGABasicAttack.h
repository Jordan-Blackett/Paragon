// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ParagonDebugConsoleVariables.h"
#include "ParagonGABasicAttack.generated.h"

extern TAutoConsoleVariable<int32> CVarDebugBasicAttack;

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	TArray<TSubclassOf<UGameplayEffect>> HitGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	TArray<TSubclassOf<UGameplayEffect>> HitGameplayCueEffects;

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

	/** Weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(UParagonGABasicAttack::FireWeapon, );

	/** Handle weapon fire */
	void HandleFiring();

protected:

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponData WeaponConfig;

	int BurstCount = 1;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;

};
