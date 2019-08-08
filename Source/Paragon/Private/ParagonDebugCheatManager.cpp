// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonDebugCheatManager.h"
#include "ParagonCharacter.h"
#include "Kismet/GameplayStatics.h"

void UParagonDebugCheatManager::SetPlayerLevel(int32 level)
{
	ACharacter* character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (character)
	{
		Cast<AParagonCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->LevelUp(level);
	}
}
