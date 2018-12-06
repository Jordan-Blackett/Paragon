// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonCharacter_Gadget.h"

#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
//#include "ParagonProjectile.h"
#include "AbilitySystemComponent.h"

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

	if (AbilitySystem)
	{
		if (HasAuthority())
		{
			for (int i = 0; i < AbilitiesSlots.Num(); i++) {
				//FGameplayAbilitySpecHandle eqe = new FGameplayAbilitySpecHandle();
				AbilitiesHandles.Add(AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilitiesSlots[i].GetDefaultObject())));
				//AbilitiesHandles[i] = AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilitiesSlots[i].GetDefaultObject()));
			}
		}
		AbilitySystem->InitAbilityActorInfo(this, this);
	}
}

void AParagonCharacter_Gadget::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AParagonCharacter_Gadget::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AParagonCharacter_Gadget::OnStopFire);

	PlayerInputComponent->BindAction("Ability_1", IE_Pressed, this, &AParagonCharacter_Gadget::AbilitySlot0);
	PlayerInputComponent->BindAction("Ability_2", IE_Pressed, this, &AParagonCharacter_Gadget::AbilitySlot1);
	PlayerInputComponent->BindAction("Ability_3", IE_Pressed, this, &AParagonCharacter_Gadget::AbilitySlot2);
	PlayerInputComponent->BindAction("Ability_4", IE_Pressed, this, &AParagonCharacter_Gadget::AbilitySlot3);
	//PlayerInputComponent->BindAction("AbilitySlot4", IE_Pressed, this, &AParagonCharacter_Gadget::AbilitySlot4);
	//PlayerInputComponent->BindAction("AbilitySlot5", IE_Pressed, this, &AParagonCharacter_Gadget::AbilitySlot5);
}

//////////////////////////////////////////////////////////////////////////
// Ability 1 - Plasma Blast


void AParagonCharacter_Gadget::OnStartFire()
{
	if (bAbilityPressed)
	{
		HideAbilityIndicator();

		if (Ability2Animation)
		{
			PlayAnimMontage(Ability2Animation);
		}

		// Cast ability

		if (AbilitiesSlots.Num() > CurrentActiveAbility)
		{
			//if (AbilitySystem->TryActivateAbilityByClass(AbilitiesSlots[CurrentActiveAbility]))
			if (AbilitySystem->TryActivateAbility(AbilitiesHandles[CurrentActiveAbility]))
			{
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Ability Failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Ability In Array"));
		}

		bAbilityPressed = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Your 2"));
		StopAnimMontage(Ability2Animation);
		StartWeaponFire();
	}
}

void AParagonCharacter_Gadget::OnStopFire()
{
	//StopWeaponFire();
}

void AParagonCharacter_Gadget::StartWeaponFire()
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

void AParagonCharacter_Gadget::StopWeaponFire()
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


void AParagonCharacter_Gadget::StartAbility1()
{	
	//if (bHasNoCooldwn && !OnCoolDown)
	if (!bAbilityPressed)
	{
		// Spawn decal / change decal
		if(AbilitiesIndicator.Num() > CurrentActiveAbility)
		{
			SpawnAbilityIndicator(AbilitiesIndicator[CurrentActiveAbility]);
		}

		bAbilityPressed = true;
	}
}

void AParagonCharacter_Gadget::ActivateAbilityInSlot(int32 Slot)
{
	if (AbilitySystem)
	{
		AbilitySystem->TryActivateAbilityByClass(AbilitiesSlots[Slot]);
	}
}

void AParagonCharacter_Gadget::AbilitySlot0()
{
	CurrentActiveAbility = 0;
	StartAbility1();
}

void AParagonCharacter_Gadget::AbilitySlot1()
{
	ActivateAbilityInSlot(1);
}

void AParagonCharacter_Gadget::AbilitySlot2()
{
	ActivateAbilityInSlot(2);
}

void AParagonCharacter_Gadget::AbilitySlot3()
{
	ActivateAbilityInSlot(3);
}

void AParagonCharacter_Gadget::AbilitySlot4()
{
	ActivateAbilityInSlot(4);
}

void AParagonCharacter_Gadget::AbilitySlot5()
{
	ActivateAbilityInSlot(5);
}

//
//float AParagonBasicAttack::PlayWeaponAnimation(UAnimMontage* Animation)
//{
//	float Duration = 0.0f;
//	if (MyPawn)
//	{
//		Duration = MyPawn->PlayAnimMontage(Animation);
//	}
//	else if (GetOwner())
//	{
//		AParagonCharacter* PlayerCharacter = Cast<AParagonCharacter>(GetOwner());
//		Duration = PlayerCharacter->PlayAnimMontage(Animation);
//	}
//
//	return Duration;
//}


// --- Netcode ---

void AParagonCharacter_Gadget::Server_FireBasicAttack_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
}

bool AParagonCharacter_Gadget::Server_FireBasicAttack_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}
