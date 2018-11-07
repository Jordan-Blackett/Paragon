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

	/** perform initial setup */
	virtual void PostInitializeComponents() override;

	// --- Abilities ---

	// Ability Basic Attack - Plasma Blast
	UPROPERTY(Replicated)
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

	//////////////////////////////////////////////////////////////////////////
	// Ability 1 - Burst Fire

	//Fires a hitscan
	void OnStartFire();
	void OnStopFire();

	/** [local] starts/stops weapon fire */
	void StartWeaponFire();
	void StopWeaponFire();

	bool bWantsToFire = false;

	void ApplyWeaponConfig(FWeaponData& WeaponDataOut, FInstantWeaponData& HitscanDataOut);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Health, Category = "Stats")
	float DummyHealth;

	/** Called when the Health variable gets updated */
	virtual void OnRep_Health(float OldHealth) override;

};
