// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonCharacter_LtBelica.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"

AParagonCharacter_LtBelica::AParagonCharacter_LtBelica()
{
	//BasicAttack = CreateDefaultSubobject<AParagonBasicAttack_HitScan>(TEXT("BasicAttack"));
	//BasicAttack->SetReplicates(true);
	//BasicAttack->InitBasicAttack(Cast<AParagonCharacter>(this));
}

void AParagonCharacter_LtBelica::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters SpawnInfo;
		BasicAttack = GetWorld()->SpawnActor<AParagonBasicAttack_HitScan>(AParagonBasicAttack_HitScan::StaticClass(), SpawnInfo);
		BasicAttack->InitBasicAttack(Cast<AParagonCharacter>(this));
		BasicAttack->SetWeaponCongfig(WeaponConfig, HitScanConfig);
	}
}

void AParagonCharacter_LtBelica::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		////GetOuter()
		//	//BasicAttack = NewObject<AParagonBasicAttack_HitScan>(this, AParagonBasicAttack_HitScan::StaticClass(), TEXT("BasicAttack"));
		//	//BasicAttack = NewObject<AParagonBasicAttack_HitScan>(this, AParagonBasicAttack_HitScan::StaticClass(), TEXT("BasicAttack"));
		//FActorSpawnParameters SpawnInfo;
		//BasicAttack = GetWorld()->SpawnActor<AParagonBasicAttack_HitScan>(AParagonBasicAttack_HitScan::StaticClass(), SpawnInfo);


		////BasicAttack.SetReplicates(true);
		//BasicAttack->InitBasicAttack(Cast<AParagonCharacter>(this));

		//// Ability Basic Attack - Burst Fire
		//BasicAttack->SetWeaponCongfig(WeaponConfig, HitScanConfig);
	}
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

void AParagonCharacter_LtBelica::OnRep_Health()
{
}

void AParagonCharacter_LtBelica::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AParagonCharacter_LtBelica, BasicAttack, COND_OwnerOnly);
}