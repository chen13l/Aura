// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetSpellDescription(int32 Level)
{
	bCallGetNextLevelinfo = false;
	return GetFireBoltDescription(Level);
}

FString UAuraFireBolt::GetNextLeveSpellDescription(int32 Level)
{
	bCallGetNextLevelinfo = true;
	return GetFireBoltDescription(Level);
}

FString UAuraFireBolt::GetFireBoltDescription(int32 Level)
{
	AbilityName = FString("Fire Bolt");
	NextLevelString = bCallGetNextLevelinfo ? FString("Next Level:") : FString("Level");
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>%s </>\n\n"

		// Level
		"<Small>%s </><Level>%d</>\n"
		// ManaCost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Number of FireBolts
		"<Default>Launches %d bolts of fire, "
		"exploding on impact and dealing: </>"

		// Damage
		"<Damage>%d</><Default> fire damage with"
		" a chance to burn</>"),

	                       // Values
	                       *AbilityName,
	                       *NextLevelString,
	                       Level,
	                       ManaCost,
	                       Cooldown,
	                       FMath::Min(Level, NumProjectiles),
	                       Damage);
}
