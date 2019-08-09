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
#include "ParagonDebugConsoleVariables.h"

extern TAutoConsoleVariable<int32> CVarDebugBasicAttack;

// Sets default values
AParagon_Gadget_SpiderMine::AParagon_Gadget_SpiderMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetSphereRadius(200.0f);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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

		// Set Initial Spawn Location
		InitLocation = GetActorLocation();

		// Set Target Location
		FVector TargetLocation2 = Cast<AParagonCharacter>(Instigator)->GetAbilityPoint();
		TargetLocation2.Z += 100; // TODO: Variable
		SetAbilityPoint(TargetLocation2);

		Init();

		GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::BotDistance, 0.02f, true);
	}
}

void AParagon_Gadget_SpiderMine::Init()
{
	if (Instigator)
	{
		// Set direction
		FVector Direction = TargetLocation - InitLocation;
		Direction = FVector(Direction.X, Direction.Y, Direction.Z);
		Direction.Normalize();
		MovementComponent->SetVelocityInLocalSpace(Direction * MovementComponent->InitialSpeed);

		// Set Rotation
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(InitLocation, TargetLocation);
		Rotation = FRotator(0, Rotation.Yaw + 90, 0);
		//SetActorRotation(Rotation);
		SkeletalMeshComponent->SetRelativeRotation(Rotation);

		MovementComponent->UpdateComponentToWorld();
		MovementComponent->Activate();

		TargetDistance = FVector::Dist(FVector(InitLocation.X, InitLocation.Y, 0), FVector(TargetLocation.X, TargetLocation.Y, 0));

		// Debug
		int32 Debug = CVarDebugBasicAttack.GetValueOnGameThread();
		if (Debug)
		{
			DrawDebugLine(GetWorld(), InitLocation, FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z), FColor::Green, true, 4.0f);
			DrawDebugLine(GetWorld(), FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z), FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z - 100), FColor::Green, true, 4.0f);
			DrawDebugPoint(GetWorld(), FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z), 10.f, FColor::Green, true, 4.0f);
			DrawDebugPoint(GetWorld(), FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z - 100), 10.f, FColor::Green, true, 4.0f);
		}
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
			SetActorLocation(FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z));
			FVector Direction = FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z - 100) - GetActorLocation();
			Direction = FVector(0, 0, Direction.Z);
			Direction.Normalize();
			MovementComponent->InitialSpeed = 500; // Todo: variable
			MovementComponent->SetVelocityInLocalSpace(Direction * MovementComponent->InitialSpeed);
			bFalling = true;
			return;
		}

		// Reached Target
		if (UKismetMathLibrary::NearlyEqual_FloatFloat(GetActorLocation().Z, TargetLocation.Z - 100, 40))
		{
			SetActorLocation(FVector(TargetLocation.X, TargetLocation.Y, (TargetLocation.Z - 100) + 20)); // TODO: mesh size/2
			MovementComponent->StopMovementImmediately();

			if (!MineTriggered)
			{
				// Check Overlapping Pawns
				if (Role == ROLE_Authority)
				{
					TArray<TEnumAsByte<EObjectTypeQuery>> CollisionType;
					CollisionType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
					TSubclassOf<AParagonCharacter> ActorFilter;
					TArray<AActor*> ActorsIgnore;
					ActorsIgnore.Add(Instigator);
					TArray<AActor*> OverlappedActors;

					UKismetSystemLibrary::SphereOverlapActors(GetWorld(), TargetLocation, CollisionComponent->GetUnscaledSphereRadius(), CollisionType, ActorFilter, ActorsIgnore, OverlappedActors); // TODO: explosion range

					if (OverlappedActors.Num() > 0) {
						for (AActor* Actor : OverlappedActors)
						{
							// Get closest

							// Set new travel location

							// Set Initial Spawn Location
							InitLocation = GetActorLocation();

							// Set Target Location
							FVector TargetLocation2 = Actor->GetActorLocation();
							TargetLocation2.Z += 100; // TODO: over socket name
							SetAbilityPoint(TargetLocation2);

							Init();
							MineTriggered = true;
						}
					}
					else
					{
						GetWorldTimerManager().ClearTimer(RangeTimerHandle);
						GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::Explode, FuzeTime, false);
						BotArmed = true;
					}
				}
			}
			else
			{
				GetWorldTimerManager().ClearTimer(RangeTimerHandle);
				GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::Explode, FuzeTime, false);
			}

			// Debug
			int32 Debug = CVarDebugBasicAttack.GetValueOnGameThread();
			if (Debug)
			{
				DrawDebugSphere(GetWorld(), GetActorLocation(), CollisionComponent->GetUnscaledSphereRadius(), 24, FColor::Green, true, 4.f);
			}
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

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), TargetLocation, 400, CollisionType, ActorFilter, ActorsIgnore, OverlappedActors); // TODO: explosion range
		
		for (AActor* Actor : OverlappedActors)
		{
			Cast<AParagonCharacter>(Actor)->TakeDamageEffectSpecs(TargetGameplayEffectSpecs);
		}

		// Debug
		int32 Debug = CVarDebugBasicAttack.GetValueOnGameThread();
		if (Debug)
		{
			DrawDebugSphere(GetWorld(), TargetLocation, 400, 24, FColor::Red, true, 4.f);
		}
	}

	Destroy();
}

void AParagon_Gadget_SpiderMine::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority && BotArmed)
	{
		if (OtherActor && OtherActor != Instigator)
		{
			GetWorldTimerManager().ClearTimer(RangeTimerHandle);

			// Set Initial Spawn Location
			InitLocation = GetActorLocation();

			// Set Target Location
			FVector TargetLocation2 = OtherActor->GetActorLocation();
			TargetLocation2.Z += 100; // TODO: over socket name
			SetAbilityPoint(TargetLocation2);

			Init();
			GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::BotDistance, 0.02f, true);
			MineTriggered = true;
		}
	}
}

