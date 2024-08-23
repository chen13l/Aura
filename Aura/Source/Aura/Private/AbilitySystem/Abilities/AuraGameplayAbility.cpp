// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetSpellDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name", Level);
}

FString UAuraGameplayAbility::GetNextLeveSpellDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name", Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell locked until </><Level>%d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(float Level) const
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo ModifierInfo : CostEffect->Modifiers)
		{
			if (ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, ManaCost);
				break;
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float Level) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, Cooldown);
	}
	return Cooldown;
}

