// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParagonUserWidget.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOwningActorChanged, AActor*, NewOwner);

/**
* Base class for UMG Widgets that belong to a single Actor in the world via a WidgetComponent, eg. for 3D health-bars, nameplate, interaction tooltip.
*/
UCLASS()
class PARAGON_API UParagonUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	/* Actor that widget is attached to via WidgetComponent */
	UPROPERTY(BlueprintReadOnly, Category = "ActorWidget")
	AActor* OwningActor;

public:

	/* Set the owning actor so widgets have access to whatever is, broadcasting OnOwningActorChanged event */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetOwningActor(AActor* NewOwner);

	//UPROPERTY(BlueprintAssignable, Category = "UI")
	//FOnOwningActorChanged OnOwningActorChanged;
};
