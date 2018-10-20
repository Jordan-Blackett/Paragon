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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AParagonCharacter_Gadget::AttemptOnFire);
}

void AParagonCharacter_Gadget::AttemptOnFire()
{
	BasicAttack->FireWeapon();
}

void AParagonCharacter_Gadget::OnFire(FRotator SpawnRotation, FVector SpawnLocation)
{
	//// Try and fire a projectile
	//if (Ability1_ProjectileClass != NULL)
	//{
	//	UWorld* const World = GetWorld();
	//	if (World != NULL)
	//	{
	//		// Set Spawn Collision Handling Override
	//		FActorSpawnParameters ActorSpawnParams;
	//		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//																										   // Spawn the projectile at the muzzle
	//		//World->SpawnActor<ABorderlandsClone2Projectile>(Ability1_ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	//	}
	//}

	//// Try and play the sound if specified
	//if (Ability1_FireSound != NULL)
	//{
	//	//UGameplayStatics::PlaySoundAtLocation(this, Ability1_FireSound, GetActorLocation());
	//}

	////UAnimInstance* AnimInstance = character->GetAnimInstance();
	//// try and play a firing animation if specified
	////if (FireAnimation != NULL)
	////{
	////	// Get the animation object for the arms mesh
	////	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	////	if (AnimInstance != NULL)
	////	{
	////		AnimInstance->Montage_Play(FireAnimation, 1.f);
	////	}
	////}
}

// --- Netcode ---

void AParagonCharacter_Gadget::Server_FireBasicAttack_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
}

bool AParagonCharacter_Gadget::Server_FireBasicAttack_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}