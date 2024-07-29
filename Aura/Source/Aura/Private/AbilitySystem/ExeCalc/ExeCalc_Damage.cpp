// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExeCalc/ExeCalc_Damage.h"

#include "AbilitySystem/AuraAttributeSet.h"

//结构体中的宏与在MMC中进行捕获类似
struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
	}
};

static const AuraDamageStatics DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExeCalc_Damage::UExeCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UExeCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EveParameters;
	EveParameters.TargetTags = TargetTags;
	EveParameters.SourceTags = SourceTags;

	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EveParameters, Armor);
	Armor = FMath::Max(0.f, Armor);
	++Armor;

	const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
