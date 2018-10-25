// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParagonCharacter.h"
#include "ParagonBasicAttack_Projectile.h"
#include "ParagonCharacter_Gadget.generated.h"

/** Gadget
* Abilties
* Ability Basic Attack - Plasma Blast
*/
UCLASS()
class PARAGON_API AParagonCharacter_Gadget : public AParagonCharacter
{
	GENERATED_BODY()
	
public:
	AParagonCharacter_Gadget();

	// --- Abilities ---

	// Ability Basic Attack - Plasma Blast
	UPROPERTY()
	AParagonBasicAttack_Projectile* BasicAttack;

	// Basic Attack Config
	UPROPERTY(EditDefaultsOnly, Category = Ability_BasicAttack)
	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = Ability_BasicAttack)
	FProjectileWeaponData ProjectileConfig;

protected:
	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	// --- Abilities ---

	//////////////////////////////////////////////////////////////////////////
	// Ability 1 - Plasma Blast

	// Fires a projectile
	void OnStartFire();
	void OnStopFire();

	/** [local] starts/stops weapon fire */
	void StartWeaponFire();
	void StopWeaponFire();

	bool bWantsToFire = false;

	// Netcode - Ability 1 - Plasma Blast

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_FireBasicAttack(FVector Origin, FVector_NetQuantizeNormal ShootDir);
	void Server_FireBasicAttack_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir);
	bool Server_FireBasicAttack_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir);
};
