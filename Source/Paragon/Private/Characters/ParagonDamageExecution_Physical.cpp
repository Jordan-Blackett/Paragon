// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonDamageExecution_Physical.h"
#include "AbilitySystemComponent.h"
#include "ParagonAttributeSet.h"
#include "ParagonCharacter.h"

struct ParagonDamageStaticsPhysical
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AbilityDamageType);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AbilityDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AbilityPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AbilityScaling);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmourPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDefense);

	ParagonDamageStaticsPhysical()
	{
		// Snapshotted
		DEFINE_ATTRIBUTE_CAPTUREDEF(UParagonAttributeSet, AbilityDamageType, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UParagonAttributeSet, AbilityDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UParagonAttributeSet, AbilityPower, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UParagonAttributeSet, AbilityScaling, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UParagonAttributeSet, ArmourPenetration, Source, true);

		// Not snapshotted
		DEFINE_ATTRIBUTE_CAPTUREDEF(UParagonAttributeSet, BaseDefense, Target, false);
	}
};

static const ParagonDamageStaticsPhysical& DamageStatics()
{
	static ParagonDamageStaticsPhysical DmgStatics;
	return DmgStatics;
}

UParagonDamageExecution_Physical::UParagonDamageExecution_Physical()
{
	RelevantAttributesToCapture.Add(DamageStatics().AbilityDamageTypeDef);
	InvalidScopedModifierAttributes.Add(DamageStatics().AbilityDamageTypeDef);
	RelevantAttributesToCapture.Add(DamageStatics().AbilityDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().AbilityPowerDef);
	InvalidScopedModifierAttributes.Add(DamageStatics().AbilityPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().AbilityScalingDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmourPenetrationDef);
	InvalidScopedModifierAttributes.Add(DamageStatics().ArmourPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BaseDefenseDef);
	InvalidScopedModifierAttributes.Add(DamageStatics().BaseDefenseDef);
}

void UParagonDamageExecution_Physical::Execute_Implementation(const FGameplayEffectCustomExecutionParameters & ExecutionParams, OUT FGameplayEffectCustomExecutionOutput & OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->AvatarActor : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->AvatarActor : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// --------------------------------------
	// Damage Done = Ability Damage * (100/(100 + Damage Reduction))
	// --------------------------------------
	// Damage Reduction = (Ability Defense - armor penetration) / (100 + (Ability Defense - armor penetration) + 10 * (Hero Level - 1))
	// --------------------------------------

	float PhysicalDefense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDefenseDef, EvaluationParameters, PhysicalDefense);
	float ArmourPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmourPenetrationDef, EvaluationParameters, ArmourPenetration);
	float HeroLevel = Cast<AParagonCharacter>(TargetActor)->GetCharacterLevel();

	float DamageReduction = (PhysicalDefense - ArmourPenetration) / (100 + (PhysicalDefense - ArmourPenetration) + (10 * (HeroLevel - 1)));
	DamageReduction *= 100;

	float AbilityDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AbilityDamageDef, EvaluationParameters, AbilityDamage);
	float AbilityPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AbilityPowerDef, EvaluationParameters, AbilityPower);
	float AbilityScaling = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AbilityScalingDef, EvaluationParameters, AbilityScaling);

	float Damage = AbilityDamage + (AbilityPower * AbilityScaling);
	float DamageDone = Damage * (100 / (100 + (DamageReduction)));

	float DamageType = 1.f;

	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().AbilityDamageProperty, EGameplayModOp::Override, DamageDone));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().AbilityDamageTypeProperty, EGameplayModOp::Override, DamageType));
	}
}




