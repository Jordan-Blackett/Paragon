// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ParagonAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class PARAGON_API UParagonAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UParagonAttributeSet();
	//virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Hitpoints
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, Health)

	// Max Hitpoints
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, MaxHealth)

	// Health Regen
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, HealthRegen)

	// Mana
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, Mana)

	// Max Mana
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, MaxMana)

	// Mana Regen
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_ManaRegen)
	FGameplayAttributeData ManaRegen;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, ManaRegen)

	// Attack Damage
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, AttackDamage)

	// Attack Speed
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, AttackSpeed)

	// Ability Defense
	UPROPERTY(BlueprintReadOnly, Category = "Defense", ReplicatedUsing = OnRep_AbilityDefense)
	FGameplayAttributeData AbilityDefense;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, AbilityDefense)

	// Base Defense
	UPROPERTY(BlueprintReadOnly, Category = "Defense", ReplicatedUsing = OnRep_BaseDefense)
	FGameplayAttributeData BaseDefense;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, BaseDefense)

	// Movement Speed
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UParagonAttributeSet, MoveSpeed)

protected:
	/** Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes. (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
	//void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	// These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	UFUNCTION()
	virtual void OnRep_Health();

	UFUNCTION()
	virtual void OnRep_MaxHealth();


	UFUNCTION()
	virtual void OnRep_HealthRegen();

	UFUNCTION()
	virtual void OnRep_Mana();

	UFUNCTION()
	virtual void OnRep_MaxMana();

	UFUNCTION()
	virtual void OnRep_ManaRegen();

	UFUNCTION()
	virtual void OnRep_AttackDamage();

	UFUNCTION()
	virtual void OnRep_AttackSpeed();

	UFUNCTION()
	virtual void OnRep_AbilityDefense();

	UFUNCTION()
	virtual void OnRep_BaseDefense();

	UFUNCTION()
	virtual void OnRep_MoveSpeed();
	
};
