// Fill out your copyright notice in the Description page of Project Settings.

#include "Paragon_Gadget_SpiderMine.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "ParagonCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Paragon.h"
#include "Kismet/GameplayStatics.h"
#include "ParagonExplosionEffect.h"

// Sets default values
AParagon_Gadget_SpiderMine::AParagon_Gadget_SpiderMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AParagon_Gadget_SpiderMine::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->MoveIgnoreActors.Add(Cast<AActor>(Instigator));

	MovementComponent->UpdateComponentToWorld();
	//MovementComponent->SetVelocityInLocalSpace(FVector(1000.f, 0.f, 0.f));
	MovementComponent->Activate();

	if (bSendOverlapEvents)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AParagon_Gadget_SpiderMine::OnOverlap);
	}

	if (bSendHitEvents)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &AParagon_Gadget_SpiderMine::OnHit);
	}

	InitLocation = GetActorLocation();
	
	GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::BotDistance, 0.02f, true);
}

void AParagon_Gadget_SpiderMine::Init()
{
	if (Instigator)
	{
		// Set direction
		FVector Direction = TargetLocation - InitLocation;
		Direction = FVector(Direction.X, Direction.Y, 0);
		Direction.Normalize();
		MovementComponent->SetVelocityInLocalSpace(Direction * MovementComponent->InitialSpeed);

		// Set Rotation
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(InitLocation, TargetLocation);
		Rotation = FRotator(0, Rotation.Yaw + 90, 0);
		SetActorRotation(Rotation);

		// Debug
		//const FVector StartTrace2 = GetActorLocation();
		//const FVector EndTrace2 = StartTrace2 + Direction * 1000;

		//DrawDebugLine(GetWorld(), StartTrace2, EndTrace2, FColor::Yellow, true);
		//DrawDebugPoint(GetWorld(), FVector(TargetLocation.X, TargetLocation.Y, 0), 20.f, FColor::Yellow, true);
		//DrawDebugPoint(GetWorld(), TargetLocation, 20.f, FColor::Yellow, true);

		TargetDistance = FVector::Dist(FVector(InitLocation.X, InitLocation.Y, 0), FVector(TargetLocation.X, TargetLocation.Y, 0));
	}
}

void AParagon_Gadget_SpiderMine::BotDistance()
{
	float Distance = FVector::Dist(InitLocation, GetActorLocation());
	if (Distance >= TargetDistance || bFalling)
	{
		if (!bFalling)
		{
			// Fall
			SetActorLocation(FVector(TargetLocation.X, TargetLocation.Y, GetActorLocation().Z));
			FVector Direction = TargetLocation - GetActorLocation();
			Direction = FVector(0, 0, Direction.Z);
			Direction.Normalize();
			MovementComponent->InitialSpeed = 1000;
			MovementComponent->SetVelocityInLocalSpace(Direction * MovementComponent->InitialSpeed);
			bFalling = true;
		}

		if (UKismetMathLibrary::NearlyEqual_FloatFloat(GetActorLocation().Z, TargetLocation.Z, 40))
		{
			SetActorLocation(FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z + 20)); // mesh size/2
			GetWorldTimerManager().ClearTimer(RangeTimerHandle);
			MovementComponent->StopMovementImmediately();
			
			GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::Explode, FuzeTime, false);
		}
	}
}

void AParagon_Gadget_SpiderMine::Explode()
{
	if (ExplosionTemplate)
	{
		FTransform const SpawnTransform(GetActorRotation(), GetActorLocation());
		AParagonExplosionEffect* const EffectActor = GetWorld()->SpawnActorDeferred<AParagonExplosionEffect>(ExplosionTemplate, SpawnTransform);
		if (EffectActor)
		{
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}

	//if (WeaponConfig.ExplosionDamage > 0 && WeaponConfig.ExplosionRadius > 0 && WeaponConfig.DamageType && Role == ROLE_Authority)
	//{
		//UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, NudgedImpactLocation, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get());
	//}
	
	FGameplayEventData Data;
	//Data.Target = OtherActor;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.Overlap"))), Data);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.ProjectileDestruction"))), FGameplayEventData());

	Destroy();
}

