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

void AParagonCharacter_LtBelica::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AParagonCharacter_LtBelica::BeginPlay()
{
	Super::BeginPlay();
}

void AParagonCharacter_LtBelica::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}