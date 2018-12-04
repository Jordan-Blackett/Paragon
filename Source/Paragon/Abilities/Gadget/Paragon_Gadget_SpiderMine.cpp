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
	
	GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::BotDistance, 0.1f, true);
}

void AParagon_Gadget_SpiderMine::Init()
{
	if (Instigator)
	{
		FVector Direction = TargetLocation - InitLocation;
		FVector Direction2 = FVector(Direction.X, Direction.Y, 0);
		Direction2.Normalize();
		MovementComponent->SetVelocityInLocalSpace(Direction2 * MovementComponent->InitialSpeed);

		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(InitLocation, TargetLocation);
		FRotator Rot2 = FRotator(0, Rot.Yaw + 90, 0);
		SetActorRotation(Rot2);

		const FVector StartTrace = GetActorLocation();
		const FVector EndTrace = StartTrace + Direction2 * 1000;

		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Blue, true);
		DrawDebugPoint(GetWorld(), TargetLocation, 20.f, FColor::Blue, true);
		//FRotator Rot = FRotationMatrix::MakeFromX(Direction2).Rotator();
	}

	TargetDistance = FVector::Dist(InitLocation, TargetLocation);
}

void AParagon_Gadget_SpiderMine::OnOverlap(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor->GetUniqueID() != Instigator->GetUniqueID())
	{
		FGameplayEventData Data;
		Data.Target = OtherActor;
		//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.Overlap"))), Data);
		//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.ProjectileDestruction"))), FGameplayEventData());
		MovementComponent->StopMovementImmediately();
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Destroy();
	}
}

void AParagon_Gadget_SpiderMine::OnHit(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != Instigator)
	{
		FGameplayEventData Data;
		Data.Target = OtherActor;
		//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.Hit"))), Data);
		//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.ProjectileDestruction"))), FGameplayEventData());
		MovementComponent->StopMovementImmediately();
		Destroy();
	}
}

void AParagon_Gadget_SpiderMine::BotDistance()
{
	//UE_LOG(LogTemp, Warning, TEXT("Your TARGET"));
	//if (Instigator)
	//{
		float Distance = FVector::Dist(InitLocation, GetActorLocation());
		 // Instigator->getmousedist or new trace
		//UE_LOG(LogTemp, Warning, TEXT("M %f"), Distance);
		if (Distance >= TargetDistance)
		{
			// stop timer
			// activemode();
			GetWorldTimerManager().ClearTimer(RangeTimerHandle);
			MovementComponent->StopMovementImmediately();
		}
	//}

	//AParagonCharacter* const PlayerController = Cast<AParagonCharacter>(GetInstigatorController());
	//if (PlayerController)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Your test"));
	//	float Distance = (PlayerController->GetActorLocation() - GetActorLocation()).SizeSquared();
	//	float TargetDistance = 50.f;
	//	if (Distance >= TargetDistance)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Your message"));
	//		MovementComponent->StopMovementImmediately();
	//	}
	//}
}

