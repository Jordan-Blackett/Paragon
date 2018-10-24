// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonCharacter_LtBelica.h"

#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"

AParagonCharacter_LtBelica::AParagonCharacter_LtBelica()
{
}

void AParagonCharacter_LtBelica::BeginPlay()
{
	Super::BeginPlay();

	BasicAttack = NewObject<AParagonBasicAttack_HitScan>(this, AParagonBasicAttack_HitScan::StaticClass(), TEXT("BasicAttack"));
	BasicAttack->InitBasicAttack(Cast<AParagonCharacter>(this));

	// Ability Basic Attack - Burst Fire
	BasicAttack->SetWeaponCongfig(WeaponConfig, HitScanConfig);
}

void AParagonCharacter_LtBelica::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AParagonCharacter_LtBelica::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AParagonCharacter_LtBelica::OnStopFire);
}

//////////////////////////////////////////////////////////////////////////
// Ability 1 - Burst Fire

void AParagonCharacter_LtBelica::OnStartFire()
{
	StartWeaponFire();
}

void AParagonCharacter_LtBelica::OnStopFire()
{
	StopWeaponFire();
}

void AParagonCharacter_LtBelica::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (BasicAttack)
		{
			BasicAttack->StartFire();
		}
	}
}

void AParagonCharacter_LtBelica::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (BasicAttack)
		{
			BasicAttack->StopFire();
		}
	}
}

