// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Paragon_Gadget_SpiderBot.generated.h"

UCLASS()
class PARAGON_API AParagon_Gadget_SpiderBot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParagon_Gadget_SpiderBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Init();

	UFUNCTION()
	FVector GetNewPathDirection(FVector NewInitLocation, FVector NewTargetLocation);

	UFUNCTION()
	virtual void ReachedBotDistance() PURE_VIRTUAL(AParagon_Gadget_SpiderBot::ReachedBotDistance, ;);

	UFUNCTION()
	virtual void DrawDebug() PURE_VIRTUAL(AParagon_Gadget_SpiderBot::DrawDebug, ;);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "SpiderBot")
	void SetGameplayEffectSpecs(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecs) { TargetGameplayEffectSpecs = GameplayEffectSpecs; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileActor)
	class UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileActor)
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ProjectileActor)
	class USkeletalMeshComponent* SkeletalMeshComponent;
	
protected:
	UPROPERTY()
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	UPROPERTY()
	FVector InitLocation;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	float TargetDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float FlyHeight = 200.f;
};
