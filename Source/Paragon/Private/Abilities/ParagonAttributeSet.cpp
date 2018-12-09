// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonAttributeSet.h"
#include "Net/UnrealNetwork.h"

UParagonAttributeSet::UParagonAttributeSet()
	: Health(1.f)
	, MaxHealth(1.f)
	, HealthRegen(1.f)
	, Mana(1.f)
	, MaxMana(1.f)
	, ManaRegen(1.f)
	, AttackDamage(1.f)
	, AttackSpeed(1.f)
	, AbilityDefense(1.f)
	, BaseDefense(1.f)
	, MoveSpeed(1.0f)
{
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
