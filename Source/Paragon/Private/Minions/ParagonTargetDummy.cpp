// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonTargetDummy.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"

// Sets default values
AParagonTargetDummy::AParagonTargetDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AParagonTargetDummy::BeginPlay()
{
	Super::BeginPlay();
}

float AParagonTargetDummy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (Role == ROLE_Authority) {
		//Decrease the character's hp 
		//SetCurrentHealth(GetCurrentHealth() - Damage);
	}

	int health = GetHealth();

	//AbilitySystem->appl
	//SetCurrentHealth(GetCurrentHealth());



	return health;
}
