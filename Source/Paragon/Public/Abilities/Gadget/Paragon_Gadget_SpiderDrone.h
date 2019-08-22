// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Gadget/Paragon_Gadget_SpiderBot.h"
#include "Paragon_Gadget_SpiderDrone.generated.h"

/**
 * 
 */
UCLASS()
class PARAGON_API AParagon_Gadget_SpiderDrone : public AParagon_Gadget_SpiderBot
{
	GENERATED_BODY()
	
	virtual void ReachedBotDistance() override;
	
};
