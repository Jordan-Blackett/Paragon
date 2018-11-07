// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ParagonGameMode.generated.h"

UCLASS(minimalapi)
class AParagonGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AParagonGameMode();

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	
	UPROPERTY()
	class UUserWidget* CurrentWidget;
};



