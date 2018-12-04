// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ParagonAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class PARAGON_API UParagonAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// Hitpoints
	UPROPERTY(Category = "Wizard Attributes | Health", EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData Health;
	
};
