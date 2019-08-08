// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonGABasicAttack.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

void UParagonGABasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// GetCurrentAbilitySpecHandle , CurrentActorInfo, GetCurrentActivationInfo
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		HandleFiring();
	}
	
	//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UParagonGABasicAttack::HandleFiring()
{
	FireWeapon();

	// GameplayCue
	if (WeaponConfig.HitGameplayCueEffects.Num() > 0)
	{
		TArray<FGameplayEffectSpecHandle> FireEffectSpecHandle;
		FireEffectSpecHandle.Add(MakeOutgoingGameplayEffectSpec(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), WeaponConfig.HitGameplayCueEffects[0], 0));

		for (const FGameplayEffectSpecHandle& SpecHandle : FireEffectSpecHandle)
		{
			if (SpecHandle.IsValid())
			{
				ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);
			}
		}
	}

	// Refire timer
	if (WeaponConfig.bBurstFire && BurstCount < WeaponConfig.BurstShots)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_HandleFiring, this, &UParagonGABasicAttack::HandleFiring, WeaponConfig.TimeBetweenShots, false);
		BurstCount++;
	} else {
		BurstCount = 1;
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
}



