// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::AuraGameplayTags;


void FAuraGameplayTags::InitNativeGameplayTags()
{
	/*
	 * Primary Attributes
	 */
	AuraGameplayTags.Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Primary.Strength", "Increases physical damage");
	AuraGameplayTags.Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Primary.Intelligence", "Increases magical damage");
	AuraGameplayTags.Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Primary.Resilience", "Increases Armor and Armor Penetration");
	AuraGameplayTags.Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Primary.Vigor", "Increases Health");

	/*
	 * Secondary Attributes
	 */
	AuraGameplayTags.Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.Armor", "Reduces damage taken, improves Block Chance");
	AuraGameplayTags.Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.ArmorPenetration", "Ignores Percentage of enemy Armor, increases Critical Hit Chance");
	AuraGameplayTags.Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.BlockChance", "Chance to cut incoming damage in half");
	AuraGameplayTags.Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.CriticalHitChance", "Chance to double damage plus critical hit bonus");
	AuraGameplayTags.Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.CriticalHitDamage", "Bonus damage added when a critical hit is scored");
	AuraGameplayTags.Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.CriticalHitResistance", "Reduces Critical Hit Chance of attacking enemies");
	AuraGameplayTags.Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.HealthRegeneration", "Amount of Health regenerated every 1 second");
	AuraGameplayTags.Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.ManaRegeneration", "Amount of Mana regenerated every 1 second");
	AuraGameplayTags.Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.MaxHealth", "Maximum amount of Health obtainable");
	AuraGameplayTags.Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"Attributes.Secondary.MaxMana", "Maximum amount of Mana obtainable");

	/*
	 * Input Tags
	 */
	AuraGameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"InputTag.LMB", "Input Tag for left mouse button");
	AuraGameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"InputTag.RMB", "Input Tag for right mouse button");
	AuraGameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"InputTag.1", "Input Tag for 1 key");
	AuraGameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"InputTag.2", "Input Tag for 2 key");
	AuraGameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"InputTag.3", "Input Tag for 3 key");
	AuraGameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		"InputTag.4", "Input Tag for 4 key");
}