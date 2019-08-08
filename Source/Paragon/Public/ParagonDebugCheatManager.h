// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "ParagonDebugCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class PARAGON_API UParagonDebugCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
	UFUNCTION(Exec)
	void SetPlayerLevel(int32 level);
	
};
