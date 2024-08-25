// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExeCalc/ExeCalc_Damage.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterCategoryInfo.h"
#include "Interaction/CombatInterface.h"

//结构体中的宏与在MMC中进行捕获类似
struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExeCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec,
                                      FAggregatorEvaluateParameters EvaluateParameters,
                                      const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagsToDefs) const
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	for (auto& [DamageType, DebuffType] : Tags.DamageTypesToDebuff)
	{
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -.5f);
		if (TypeDamage > -.5f) // .5 padding for floating point [im]precision
		{
			// Determine if there was a successful debuff
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(Tags.Debuff_Related_Chance, false, -.5f);

			float TargetDebuffResistance = 0.f;
			const FGameplayTag& ResistanceTag = Tags.DamageTypesToResistances[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TagsToDefs[ResistanceTag], EvaluateParameters,
			                                                           TargetDebuffResistance);
			TargetDebuffResistance = FMath::Max(TargetDebuffResistance, 0.f);

			const float EffectiveDebuffChance = SourceDebuffChance * (100.f - TargetDebuffResistance) / 100.f;
			const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
			if (bDebuff)
			{
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, bDebuff);
				UAuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

				const float DebuffDamage = Spec.GetSetByCallerMagnitude(Tags.Debuff_Related_Damage, false, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(Tags.Debuff_Related_Duration, false, -1.f);
				const float DebuffFrequncy = Spec.GetSetByCallerMagnitude(Tags.Debuff_Related_Frequency, false, -1.f);

				UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequncy);
			}
		}
	}
}

void UExeCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagToCaptureDef;
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	TagToCaptureDef.Add(Tags.Attribute_Secondary_Armor, DamageStatics().ArmorDef);
	TagToCaptureDef.Add(Tags.Attribute_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagToCaptureDef.Add(Tags.Attribute_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagToCaptureDef.Add(Tags.Attribute_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagToCaptureDef.Add(Tags.Attribute_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagToCaptureDef.Add(Tags.Attribute_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

	TagToCaptureDef.Add(Tags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagToCaptureDef.Add(Tags.Attributes_Resistance_Lightning, DamageStatics().FireResistanceDef);
	TagToCaptureDef.Add(Tags.Attributes_Resistance_Arcane, DamageStatics().FireResistanceDef);
	TagToCaptureDef.Add(Tags.Attributes_Resistance_Physical, DamageStatics().FireResistanceDef);

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>()) { TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar); }
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>()) { SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar); }

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = TargetTags;
	EvaluateParameters.SourceTags = SourceTags;

	// Debuff
	DetermineDebuff(ExecutionParams, Spec, EvaluateParameters, TagToCaptureDef);

	//get Damage SetByCaller Magnitude
	float Damage = 0.f;
	for (auto& [DamageTypeTag,ResistanceTag] : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		checkf(TagToCaptureDef.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"),
		       *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagToCaptureDef[ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);

		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		DamageTypeValue *= (100.f - Resistance) / 100.f;

		Damage += DamageTypeValue;
	}

	//damage Coefficient
	const UCharacterCategoryInfo* CharacterCategoryInfo = UAuraAbilitySystemLibrary::GetCharacterCategoryInfo(SourceAvatar);
	const UCurveTable* DamageCalCoefficient = CharacterCategoryInfo->DamageCalculationCoefficient;

	/* Capture BlockChance on Target, and determine if there was a successful block
	 * if Block, Halve the Damage */
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max(0.f, TargetBlockChance);
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(ContextHandle, bBlocked);

	Damage = bBlocked ? Damage / 2.f : Damage;
	/* End Block */

	/* ArmorPenetration ignores a percentage of the Target's Armor */
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(SourceArmorPenetration, 0.f);

	const FRealCurve* ArmorPenetrationCurve = DamageCalCoefficient->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	const FRealCurve* EffectiveDamageCurve = DamageCalCoefficient->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveDamageCurve->Eval(TargetPlayerLevel);
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	/* End Armor */

	/* Critical
	 * CriticalHitResistance reduces CriticalHitChance by certain percentage
	 * double damage + CriticalHitDamage if critical hit
	 */
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max(0.f, SourceCriticalHitChance);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParameters,
	                                                           TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(0.f, TargetCriticalHitResistance);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(0.f, SourceCriticalHitDamage);

	const FRealCurve* CriticalHitResistanceCurve = DamageCalCoefficient->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	UAuraAbilitySystemLibrary::SetIsCritical(ContextHandle, bCriticalHit);

	Damage = bCriticalHit ? Damage * 2.f + SourceCriticalHitDamage : Damage;
	/* End Critical */

	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
