// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParagonCharacter.h"
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

protected:
	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
