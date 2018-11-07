// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonWidgetComponent.h"
#include "ParagonUserWidget.h"



UParagonWidgetComponent::UParagonWidgetComponent()
{
	// Set common defaults when using widgets on Actors
	SetDrawAtDesiredSize(true);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UParagonWidgetComponent::InitWidget()
{
	// Base implementation creates the 'Widget' instance
	Super::InitWidget();

	if (Widget)
	{
		if (!Widget->IsA(UParagonUserWidget::StaticClass()))
		{
			// Suggest deriving from actor widget (not strictly required, but you lose the benefit of auto-assigning the Actor this way)
			//UE_LOG(LogGame, Warning, TEXT("WidgetClass for %s does not derive from SActorWidget"), *GetNameSafe(GetOwner()));
		}

		UParagonUserWidget* WidgetInst = Cast<UParagonUserWidget>(Widget);
		if (WidgetInst)
		{
			// Assign the owner, now we have easy access in the widget itself
			WidgetInst->SetOwningActor(GetOwner());
		}
	}
}

