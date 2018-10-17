// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonTargetDummy.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
AParagonTargetDummy::AParagonTargetDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Init text render comp
	CharText = CreateDefaultSubobject<UTextRenderComponent>(FName("CharText"));
	CharText->SetupAttachment(GetRootComponent());

	UpdateCharText();
}

// Called when the game starts or when spawned
void AParagonTargetDummy::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AParagonTargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AParagonTargetDummy::UpdateCharText()
{
	//Create a string that will display the health and bomb count values
	FString NewText = FString("Health: ") + FString::SanitizeFloat(GetCurrentHealth());

	//Set the created string to the text render comp
	CharText->SetText(FText::FromString(NewText));
}

float AParagonTargetDummy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (Role == ROLE_Authority) {
		//Decrease the character's hp 
		SetCurrentHealth(GetCurrentHealth() - Damage);

		UpdateCharText();
	}

	int health = GetCurrentHealth();

	return health;
}

// Netcode

void AParagonTargetDummy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AParagonTargetDummy, CurrentHealth);
}

void AParagonTargetDummy::OnRep_Health()
{
	UpdateCharText();
}
