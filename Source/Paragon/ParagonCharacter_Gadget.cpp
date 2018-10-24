// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonCharacter_Gadget.h"

#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
//#include "ParagonProjectile.h"

AParagonCharacter_Gadget::AParagonCharacter_Gadget()
{
	// --- Abilities ---
	//BasicAttack = NewObject<AParagonBasicAttack_Projectile>();
	//BasicAttack = NewObject<AParagonBasicAttack_Projectile>();
	//BasicAttack = CreateDefaultSubobject<AParagonBasicAttack_Projectile>(TEXT("Test"));
	//UHexTile* NewTile = NewObject<UHexTile>(this, UHexTile::StaticClass(), *TileName);
}

void AParagonCharacter_Gadget::BeginPlay()
{
	Super::BeginPlay();

	BasicAttack = NewObject<AParagonBasicAttack_Projectile>(this, AParagonBasicAttack_Projectile::StaticClass(), TEXT("BasicAttack"));
	BasicAttack->InitBasicAttack(Cast<AParagonCharacter>(this));

	// Ability Basic Attack - Plasma Blast
	BasicAttack->SetWeaponCongfig(WeaponConfig, ProjectileConfig);
}

void AParagonCharacter_Gadget::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind fire event
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AParagonCharacter_Gadget::OnStartFire);
	//PlayerInputComponent->BindAction("Fire", IE_Released, this, &AParagonCharacter_Gadget::OnStopFire);
}

//void AParagonCharacter_Gadget::OnStartFire()
//{
//	StartWeaponFire();
//
//	//BasicAttack->FireWeapon();
//}
//
//void AParagonCharacter_Gadget::StartWeaponFire(FRotator SpawnRotation, FVector SpawnLocation)
//{
//	if (!bWantsToFire)
//	{
//		bWantsToFire = true;
//		if (BasicAttack)
//		{
//			BasicAttack->FireWeapon();
//		}
//	}
//}
//
//void AShooterCharacter::StopWeaponFire()
//{
//	if (bWantsToFire)
//	{
//		bWantsToFire = false;
//		if (CurrentWeapon)
//		{
//			CurrentWeapon->StopFire();
//		}
//	}
//}

// --- Netcode ---

void AParagonCharacter_Gadget::Server_FireBasicAttack_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
}

bool AParagonCharacter_Gadget::Server_FireBasicAttack_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}