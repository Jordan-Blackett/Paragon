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

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AParagon_Gadget_SpiderMine::BeginPlay()
{
	Super::BeginPlay();

	if (Instigator)
	{
		CollisionComponent->MoveIgnoreActors.Add(Cast<AActor>(Instigator));

		InitLocation = GetActorLocation();

		GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::BotDistance, 0.02f, true);

		SetAbilityPoint(Cast<AParagonCharacter>(Instigator)->GetAbilityPoint());
		Init();
	}
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
		//SetActorRotation(Rotation);
		SkeletalMeshComponent->SetRelativeRotation(Rotation);

		MovementComponent->UpdateComponentToWorld();
		MovementComponent->Activate();

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

	if (Role == ROLE_Authority)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> CollisionType;
		CollisionType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TSubclassOf<AParagonCharacter> ActorFilter;
		TArray<AActor*> ActorsIgnore;
		ActorsIgnore.Add(Instigator);
		TArray<AActor*> OverlappedActors;

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), TargetLocation, 400, CollisionType, ActorFilter, ActorsIgnore, OverlappedActors);
		DrawDebugSphere(GetWorld(), TargetLocation, 400, 24, FColor::Yellow, 5, 1);
		
		for (AActor* Actor : OverlappedActors)
		{
			Cast<AParagonCharacter>(Actor)->TakeDamageEffectSpecs(TargetGameplayEffectSpecs);
		}
	}

	Destroy();
}
