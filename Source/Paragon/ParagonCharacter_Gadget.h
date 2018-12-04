// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParagonCharacter.h"
#include "ParagonBasicAttack_Projectile.h"
#include "GameplayAbilitySpec.h"
#include "ParagonCharacter_Gadget.generated.h"

//class FGameplayAbilitySpecHandle;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> AbilitiesSlots;

	TArray<FGameplayAbilitySpecHandle> AbilitiesHandles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<UMaterialInterface*> AbilitiesIndicator;

	bool bAbilityPressed;
	int CurrentActiveAbility;

	//////////////////////////////////////////////////////////////////////////
	// Ability 1 - Plasma Blast
	UPROPERTY()
	AParagonBasicAttack_Projectile* BasicAttack;

	// Basic Attack Config
	UPROPERTY(EditDefaultsOnly, Category = Ability_BasicAttack)
	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = Ability_BasicAttack)
	FProjectileWeaponData ProjectileConfig;

	//////////////////////////////////////////////////////////////////////////
	// Ability 2 - 

	UPROPERTY(EditDefaultsOnly, Category = "Ability_2")
	UAnimMontage* Ability2Animation;

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

	//////////////////////////////////////////////////////////////////////////
	// Ability 2 - 

	void StartAbility1();


	UFUNCTION()
	void AbilitySlot0();
	UFUNCTION()
	void AbilitySlot1();
	UFUNCTION()
	void AbilitySlot2();
	UFUNCTION()
	void AbilitySlot3();
	UFUNCTION()
	void AbilitySlot4();
	UFUNCTION()
	void AbilitySlot5();

	void ActivateAbilityInSlot(int32 Slot);

	// Netcode - Ability 1 - Plasma Blast

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_FireBasicAttack(FVector Origin, FVector_NetQuantizeNormal ShootDir);
	void Server_FireBasicAttack_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir);
	bool Server_FireBasicAttack_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir);
};
