// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonExplosionEffect.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AParagonExplosionEffect::AParagonExplosionEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ExplosionLightComponentName = TEXT("ExplosionLight");

	//ExplosionLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, ExplosionLightComponentName);
	//RootComponent = ExplosionLight;
	//ExplosionLight->AttenuationRadius = 400.0;
	//ExplosionLight->Intensity = 500.0f;
	//ExplosionLight->bUseInverseSquaredFalloff = false;
	//ExplosionLight->LightColor = FColor(255, 185, 35);
	//ExplosionLight->CastShadows = false;
	//ExplosionLight->bVisible = true;

	//ExplosionLightFadeOut = 0.2f;
}

// Called when the game starts or when spawned
void AParagonExplosionEffect::BeginPlay()
{
	Super::BeginPlay();
	
	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation());
	}

	//if (ExplosionSound)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	//}

	//if (Decal.DecalMaterial)
	//{
	//	FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
	//	RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

	//	UGameplayStatics::SpawnDecalAttached(Decal.DecalMaterial, FVector(Decal.DecalSize, Decal.DecalSize, 1.0f),
	//		SurfaceHit.Component.Get(), SurfaceHit.BoneName,
	//		SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
	//		Decal.LifeSpan);
	//}
}

// Called every frame
void AParagonExplosionEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TimeAlive = GetWorld()->GetTimeSeconds() - CreationTime;
	const float TimeRemaining = FMath::Max(0.0f, ExplosionLightFadeOut - TimeAlive);

	if (TimeRemaining > 0)
	{
		const float FadeAlpha = 1.0f - FMath::Square(TimeRemaining / ExplosionLightFadeOut);

		//UPointLightComponent* DefLight = Cast<UPointLightComponent>(GetClass()->GetDefaultSubobjectByName(ExplosionLightComponentName));
		//ExplosionLight->SetIntensity(DefLight->Intensity * FadeAlpha);
	}
	else
	{
		Destroy();
	}
}

