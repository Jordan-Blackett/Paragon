// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonAttributeSet.h"
#include "Net/UnrealNetwork.h"

UParagonAttributeSet::UParagonAttributeSet()
	: Health(1.f)
	//, MaxHealth(1.f)
	//, Mana(0.f)
	//, MaxMana(0.f)
	//, AttackPower(1.0f)
	//, DefensePower(1.0f)
	//, MoveSpeed(1.0f)
	//, Damage(0.0f)
{
}

void UParagonAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UParagonAttributeSet, Health);
	//DOREPLIFETIME(URPGAttributeSet, MaxHealth);
	//DOREPLIFETIME(URPGAttributeSet, Mana);
	//DOREPLIFETIME(URPGAttributeSet, MaxMana);
	//DOREPLIFETIME(URPGAttributeSet, AttackPower);
	//DOREPLIFETIME(URPGAttributeSet, DefensePower);
	//DOREPLIFETIME(URPGAttributeSet, MoveSpeed);
}

void UParagonAttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UParagonAttributeSet, Health);
}
