// Fill out your copyright notice in the Description page of Project Settings.

#include "Paragon_Gadget_SpiderBot.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "ParagonCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "ParagonDebugConsoleVariables.h"

extern TAutoConsoleVariable<int32> CVarDebugBasicAttack;

// Sets default values
AParagon_Gadget_SpiderBot::AParagon_Gadget_SpiderBot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(CollisionComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->bAutoActivate = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AParagon_Gadget_SpiderBot::BeginPlay()
{
	Super::BeginPlay();
	
	if (Instigator)
	{
		CollisionComponent->MoveIgnoreActors.Add(Cast<AActor>(Instigator));

		if (Role == ROLE_Authority)
		{
			// Set Initial Spawn Location
			InitLocation = GetActorLocation();

			// Set Target Location
			TargetLocation = Cast<AParagonCharacter>(Instigator)->GetAbilityPoint();
			TargetLocation.Z += FlyHeight;

			Init();
		}
	}
}

void AParagon_Gadget_SpiderBot::Init()
{
	// Set Direction
	FVector Direction = TargetLocation - InitLocation;
	Direction.Normalize();
	MovementComponent->SetVelocityInLocalSpace(Direction * MovementComponent->InitialSpeed);

	// Set Rotation
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(InitLocation, TargetLocation);
	Rotation = FRotator(0, Rotation.Yaw + 90, 0);
	SkeletalMeshComponent->SetRelativeRotation(Rotation);

	MovementComponent->UpdateComponentToWorld();
	MovementComponent->Activate();

	TargetDistance = FVector::Dist(InitLocation, TargetLocation);

	// Debug - TODO: virtual function
	int32 Debug = CVarDebugBasicAttack.GetValueOnGameThread();
	if (Debug)
	{
		DrawDebugLine(GetWorld(), InitLocation, TargetLocation, FColor::Green, true, 4.0f);
		DrawDebugPoint(GetWorld(), TargetLocation, 10.f, FColor::Green, true, 4.0f);
		DrawDebug();
	}
}

FVector AParagon_Gadget_SpiderBot::GetNewPathDirection(FVector NewInitLocation, FVector NewTargetLocation)
{
	// Set New Init Location & Target Location
	InitLocation = NewInitLocation;
	TargetLocation = NewTargetLocation;

	TargetDistance = FVector::Dist(InitLocation, TargetLocation);

	// Set Direction
	FVector Direction = TargetLocation - InitLocation;
	Direction.Normalize();
	return Direction;
}

// Called every frame
void AParagon_Gadget_SpiderBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority)
	{
		float Distance = FVector::Dist(InitLocation, GetActorLocation());
		if (Distance >= TargetDistance)
		{
			ReachedBotDistance();
		}
	}
}