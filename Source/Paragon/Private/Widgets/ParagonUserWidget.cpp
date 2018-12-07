// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonUserWidget.h"



void UParagonUserWidget::SetOwningActor(AActor* NewOwner)
{
	if (OwningActor == NewOwner)
	{
		// Skip repeated calls
		return;
	}

	OwningActor = NewOwner;

	//OnOwningActorChanged.Broadcast(NewOwner);
}
