// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "ParagonCharacter.h"
#include "GameFramework/Controller.h"

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
	, AbilityDamageType(0.0f)
	, AbilityScaling (0.0f)
{
}

void UParagonAttributeSet::PreAttributeChange(const FGameplayAttribute & Attribute, float & NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
}

void UParagonAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AParagonCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		//UE_LOG(LogTemp, Log, TEXT("Your message11111111111111111111"));
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AParagonCharacter>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetAbilityDamageTypeAttribute())
	{
		// Get the Source actor
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		AParagonCharacter* SourceCharacter = nullptr;
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			// Use the controller to find the source pawn
			if (SourceController)
			{
				SourceCharacter = Cast<AParagonCharacter>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<AParagonCharacter>(SourceActor);
			}

			// Set the causer actor based on context if it's set
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}

		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetAbilityDamage();
		SetAbilityDamage(0.f);

		if (LocalDamageDone > 0)
		{
			// Apply the health change and then clamp it
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetMaxHealth()));

			if (TargetCharacter)
			{
				bool DamageType = GetAbilityDamageType();

				// This is proper damage
				TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor, DamageType);

				// Call for all health changes
				TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
			}
		}
	}
}

void UParagonAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
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
