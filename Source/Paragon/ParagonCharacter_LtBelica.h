// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParagonCharacter.h"
#include "ParagonBasicAttack_HitScan.h"
#include "ParagonCharacter_LtBelica.generated.h"

/** LtBelica
 * Abilities
 * Ability Basic Attack - Burst Fire
 */
UCLASS()
class PARAGON_API AParagonCharacter_LtBelica : public AParagonCharacter
{
	GENERATED_BODY()
	
public:
	AParagonCharacter_LtBelica();

	// --- Abilities ---

	// Ability Basic Attack - Plasma Blast
	UPROPERTY()
	AParagonBasicAttack_HitScan* BasicAttack;

	// Basic Attack Config
	UPROPERTY(EditDefaultsOnly, Category = Ability_BasicAttack)
	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = Ability_BasicAttack)
	FInstantWeaponData HitScanConfig;

protected:
	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	// --- Abilities ---

	// Ability 1 - Plasma Blast

	//Fires a hitscan
	void AttemptOnFire();
	
};
