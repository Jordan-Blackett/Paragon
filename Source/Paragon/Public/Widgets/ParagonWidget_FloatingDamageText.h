// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParagonWidget_FloatingDamageText.generated.h"

/**
 * 
 */
UCLASS()
class PARAGON_API UParagonWidget_FloatingDamageText : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Init();

	UFUNCTION(BlueprintImplementableEvent)
	void SetDamageValue(float Damage);

	UFUNCTION(BlueprintImplementableEvent)
	void DamageType(bool DamageType);

	UFUNCTION(BlueprintImplementableEvent)
	void SetInitialScreenLocation(FVector2D ScreenLocation);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D InitialScreenLocation;

};
