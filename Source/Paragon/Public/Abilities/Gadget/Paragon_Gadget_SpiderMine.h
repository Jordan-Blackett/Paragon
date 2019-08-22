// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Gadget/Paragon_Gadget_SpiderBot.h"
#include "Paragon_Gadget_SpiderMine.generated.h"

class AParagonExplosionEffect;

/**
 *
 */
UCLASS()
class PARAGON_API AParagon_Gadget_SpiderMine : public AParagon_Gadget_SpiderBot
{
	GENERATED_BODY()

protected:
	virtual void ReachedBotDistance() override;

	virtual void DrawDebug() override;

	UFUNCTION()
	void Explode();

	UFUNCTION()
	void OnRep_Explode();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Effects for explosion
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class AParagonExplosionEffect> ExplosionTemplate;

	UPROPERTY()
	bool bFalling = false;

	UPROPERTY()
	bool bSpiderMineTargetTriggered = false;

	UPROPERTY()
	bool bActorTargetReached = false;

	UPROPERTY()
	bool bBotArmed = false;

	UPROPERTY()
	float HoverOffset = 25.f;

	UPROPERTY(ReplicatedUsing = OnRep_Explode)
	bool bExploded = false;

	UPROPERTY()
	FTimerHandle RangeTimerHandle;

	UPROPERTY()
	AActor* TargetedActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float ExplosionRange = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float FuzeTime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float FallSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float TargetSpeed = 500.f;
};
