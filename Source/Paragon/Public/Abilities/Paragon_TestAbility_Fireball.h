// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Paragon_TestAbility_Fireball.generated.h"

UCLASS()
class PARAGON_API AParagon_TestAbility_Fireball : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParagon_TestAbility_Fireball();
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileActor)
	class UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileActor)
	class USphereComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ProjectileActor)
	bool bSendOverlapEvents;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ProjectileActor)
	bool bSendHitEvents;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ProjectileActor)
	bool bTreatOverlapAsHit;	
};
