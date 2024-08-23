// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* Actor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	for (auto [DamageTag,DamageFloat] : DamageTypes)
	{
		float FloatMagnitude = DamageFloat.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageTag, FloatMagnitude);
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),
	                                                                          UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor));
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomAttackMontage(const TArray<FTaggedMontage> TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	return FTaggedMontage();
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType) const
{
	checkf(DamageTypes.Contains(DamageType), TEXT("Gameplay Ability [%s] does not contain DamageType [%s]"),
	       *GetNameSafe(this), *DamageType.ToString());

	return DamageTypes[DamageType].GetValueAtLevel(InLevel);
}
