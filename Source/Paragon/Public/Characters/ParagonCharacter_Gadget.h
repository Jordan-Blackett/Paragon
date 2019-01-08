// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParagonCharacter.h"
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

protected:
	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
