// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UParagonAttributeSet::UParagonAttributeSet()
	: Health(1.f)
	, MaxHealth(1.f)
	, HealthRegen(1.f)
	, Mana(1.f)
	, MaxMana(1.f)
	, ManaRegen(1.f)
	, AttackDamage(1.f)
	, AttackSpeed(1.f)
	, AbilityPower(0.f)
	, ArmourPenetration(0.f)
	, AbilityDefense(1.f)
	, BaseDefense(1.f)
	, MoveSpeed(1.0f)
	, AbilityDamage(0.0f)
	, AbilityScaling (0.0f)
{
}

void UParagonAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data)
{
	Super::PostGameplayEffectExecute(Data);

	

	//AActor* TargetActor = nullptr;
	//AController* TargetController = nullptr;
	////ARPGCharacterBase* TargetCharacter = nullptr;

	//TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
	////TargetCharacter = Cast<ARPGCharacterBase>(TargetActor);

	UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Health is %f"), GetAttackDamage());

	if (Data.EvaluatedData.Attribute == GetAbilityDamageAttribute())
	{
		const float LocalDamageDone = GetAbilityDamage();
		SetAbilityDamage(0.f);

		if (LocalDamageDone > 0)
		{
			// Apply the health change and then clamp it
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetMaxHealth()));

			//if (TargetCharacter)
			//{
			//	// This is proper damage
			//	TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);

			//	// Call for all health changes
			//	TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
			//}
		}
	}
}

void UParagonAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UParagonAttributeSet, Health);
	DOREPLIFETIME(UParagonAttributeSet, MaxHealth);
	DOREPLIFETIME(UParagonAttributeSet, HealthRegen);
	DOREPLIFETIME(UParagonAttributeSet, Mana);
	DOREPLIFETIME(UParagonAttributeSet, MaxMana);
	DOREPLIFETIME(UParagonAttributeSet, ManaRegen);
	DOREPLIFETIME(UParagonAttributeSet, AttackDamage);
	DOREPLIFETIME(UParagonAttributeSet, AttackSpeed);
	DOREPLIFETIME(UParagonAttributeSet, AbilityPower);
	DOREPLIFETIME(UParagonAttributeSet, ArmourPenetration);
	DOREPLIFETIME(UParagonAttributeSet, AbilityDefense);
	DOREPLIFETIME(UParagonAttributeSet, BaseDefense);
	DOREPLIFETIME(UParagonAttributeSet, MoveSpeed);
}

void UParagonAttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, Health);
}

void UParagonAttributeSet::OnRep_MaxHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, MaxHealth);
}

void UParagonAttributeSet::OnRep_HealthRegen()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, HealthRegen);
}

void UParagonAttributeSet::OnRep_Mana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, Mana);
}

void UParagonAttributeSet::OnRep_MaxMana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, MaxMana);
}

void UParagonAttributeSet::OnRep_ManaRegen()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, ManaRegen);
}

void UParagonAttributeSet::OnRep_AttackDamage()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, AttackDamage);
}

void UParagonAttributeSet::OnRep_AttackSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, AttackSpeed);
}

void UParagonAttributeSet::OnRep_AbilityPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, AbilityPower);
}

void UParagonAttributeSet::OnRep_ArmourPenetration()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, ArmourPenetration);
}

void UParagonAttributeSet::OnRep_AbilityDefense()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, AbilityDefense);
}

void UParagonAttributeSet::OnRep_BaseDefense()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, BaseDefense);
}

void UParagonAttributeSet::OnRep_MoveSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, MoveSpeed);
}
