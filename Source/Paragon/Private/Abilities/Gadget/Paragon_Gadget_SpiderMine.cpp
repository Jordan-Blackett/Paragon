// Fill out your copyright notice in the Description page of Project Settings.

#include "Paragon_Gadget_SpiderMine.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ParagonCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "ParagonExplosionEffect.h"
#include "ParagonDebugConsoleVariables.h"

extern TAutoConsoleVariable<int32> CVarDebugBasicAttack;

void AParagon_Gadget_SpiderMine::ReachedBotDistance()
{
	if (!bFalling)
	{
		// Fall
		SetActorLocation(TargetLocation);

		// Set new travel location
		MovementComponent->SetVelocityInLocalSpace(GetNewPathDirection(TargetLocation, FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z + (-FlyHeight + 20))) * FallSpeed);

		bFalling = true;
		return;
	}
	else
	{
		// Reached Target
		MovementComponent->StopMovementImmediately();
		SetActorLocation(TargetLocation);
		
		if (!bBotArmed)
		{
			// Check Overlapping Pawns
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
					//
					//
					TargetedActor = Actor;

					// Set new travel location
					FVector SocketLocation = Cast<AParagonCharacter>(Actor)->GetMesh()->GetSocketLocation("HoverAttachPoint");
					SocketLocation.Z += HoverOffset;
					MovementComponent->SetVelocityInLocalSpace(GetNewPathDirection(TargetLocation, SocketLocation) * TargetSpeed);

					bSpiderMineTargetTriggered = true;
				}
			}

			GetWorldTimerManager().SetTimer(RangeTimerHandle, this, &AParagon_Gadget_SpiderMine::Explode, FuzeTime, false);
			bBotArmed = true;
		}
		else if (bSpiderMineTargetTriggered && !bActorTargetReached)
		{
			bActorTargetReached = true;
		}

		// Debug
		int32 Debug = CVarDebugBasicAttack.GetValueOnGameThread();
		if (Debug)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), CollisionComponent->GetUnscaledSphereRadius(), 24, FColor::Green, true, 4.f);
		}
	}
}

void AParagon_Gadget_SpiderMine::DrawDebug()
{
	DrawDebugLine(GetWorld(), FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z), FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z + -FlyHeight), FColor::Green, true, 4.0f);
	DrawDebugPoint(GetWorld(), FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z + -FlyHeight), 10.f, FColor::Green, true, 4.0f);
}

void AParagon_Gadget_SpiderMine::Explode()
{
	bExploded = true;

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

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), TargetLocation, ExplosionRange, CollisionType, ActorFilter, ActorsIgnore, OverlappedActors);
		
		for (AActor* Actor : OverlappedActors)
		{
			Cast<AParagonCharacter>(Actor)->TakeDamageEffectSpecs(TargetGameplayEffectSpecs);
		}

		// Debug
		int32 Debug = CVarDebugBasicAttack.GetValueOnGameThread();
		if (Debug)
		{
			DrawDebugSphere(GetWorld(), TargetLocation, ExplosionRange, 24, FColor::Red, true, 4.f);
		}
	}

	SetLifeSpan(0.25f);
	//Destroy();
}

void AParagon_Gadget_SpiderMine::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority && !bSpiderMineTargetTriggered && bBotArmed)
	{
		if (OtherActor && OtherActor != Instigator)
		{
			TargetedActor = OtherActor;

			// Set new travel location
			FVector SocketLocation = Cast<AParagonCharacter>(OtherActor)->GetMesh()->GetSocketLocation("HoverAttachPoint");
			SocketLocation.Z += HoverOffset;
			MovementComponent->SetVelocityInLocalSpace(GetNewPathDirection(TargetLocation, SocketLocation) * TargetSpeed);

			bSpiderMineTargetTriggered = true;
		}
	}
}

void AParagon_Gadget_SpiderMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority)
	{
		if (TargetedActor && bActorTargetReached)
		{
			// Hover
			FVector SocketLocation = Cast<AParagonCharacter>(TargetedActor)->GetMesh()->GetSocketLocation("HoverAttachPoint");
			SocketLocation.Z += HoverOffset;
			SetActorLocation(SocketLocation);
		}
	}
}

void AParagon_Gadget_SpiderMine::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AParagon_Gadget_SpiderMine, bExploded, COND_SkipOwner);
}

void AParagon_Gadget_SpiderMine::OnRep_Explode()
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
}