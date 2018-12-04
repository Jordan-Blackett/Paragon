// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Paragon_Gadget_SpiderMine.generated.h"

UCLASS()
class PARAGON_API AParagon_Gadget_SpiderMine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParagon_Gadget_SpiderMine();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void BotDistance();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetAbilityPoint(FVector AbilityPoint) { TargetLocation = AbilityPoint; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Init();

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

private:
	UPROPERTY()
	FVector InitLocation;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	float TargetDistance;

	UPROPERTY()
	FTimerHandle RangeTimerHandle;

};
