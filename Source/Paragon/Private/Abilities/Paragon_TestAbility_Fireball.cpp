// Fill out your copyright notice in the Description page of Project Settings.

#include "Paragon_TestAbility_Fireball.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
AParagon_TestAbility_Fireball::AParagon_TestAbility_Fireball()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AParagon_TestAbility_Fireball::BeginPlay()
{
	Super::BeginPlay();
	
	if (Instigator)
	{
		SetActorRotation(Instigator->GetActorRotation());
	}
	MovementComponent->UpdateComponentToWorld();
	MovementComponent->SetVelocityInLocalSpace(FVector(1000.f, 0.f, 0.f));
	MovementComponent->Activate();

	if (bSendOverlapEvents)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AParagon_TestAbility_Fireball::OnOverlap);
	}
	if (bSendHitEvents)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &AParagon_TestAbility_Fireball::OnHit);
	}
}

void AParagon_TestAbility_Fireball::OnOverlap(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor->GetUniqueID() != Instigator->GetUniqueID())
	{
		FGameplayEventData Data;
		Data.Target = OtherActor;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.Overlap"))), Data);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.ProjectileDestruction"))), FGameplayEventData());
		MovementComponent->StopMovementImmediately();
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Destroy();
	}
}

void AParagon_TestAbility_Fireball::OnHit(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != Instigator)
	{
		FGameplayEventData Data;
		Data.Target = OtherActor;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.Hit"))), Data);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, FGameplayTag::RequestGameplayTag(FName(TEXT("Event.ProjectileDestruction"))), FGameplayEventData());
		MovementComponent->StopMovementImmediately();
		Destroy();
	}
}

