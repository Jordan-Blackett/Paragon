// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<int32> CVarDebugBasicAttack(
	TEXT("PARAGON.DebugBasicAttack"),
	0,
	TEXT("Draw Debug Lines For Line Traces.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);

/**
 * 
 */
class PARAGON_API ParagonDebugConsoleVariables
{
public:
	ParagonDebugConsoleVariables();
	~ParagonDebugConsoleVariables();
};
