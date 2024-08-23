// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	OnSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->OnAbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		if (AbilityTag.MatchesTagExact(SelectedAbility.AbilityTag))
		{
			SelectedAbility.StatusTag = StatusTag;
			bool bCanSpendPoints = false;
			bool bCanEquipped = false;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bCanSpendPoints, bCanEquipped);
			SpellGlobeSelectedDelegate.Broadcast(bCanSpendPoints, bCanEquipped);
		}
		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAuraPS()->OnSpellPointsChangeDelegate.AddLambda([this](int32 SpellPoints)
	{
		OnSpellPointsChanged.Broadcast(SpellPoints);
		CurrentSpellPoints = SpellPoints;

		bool bCanSpendPoints = false;
		bool bCanEquipped = false;
		ShouldEnableButtons(SelectedAbility.StatusTag, CurrentSpellPoints, bCanSpendPoints, bCanEquipped);
		SpellGlobeSelectedDelegate.Broadcast(bCanSpendPoints, bCanEquipped);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	FGameplayAbilitySpec* Spec = GetAuraASC()->GetAbilitySpecFromTag(AbilityTag);
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	FGameplayTag StatusTag;

	const bool bIsValid = AbilityTag.IsValid();
	const bool bNoneAbilityTag = AbilityTag.MatchesTagExact(Tags.Abilities_None);
	const bool bSpecValid = Spec != nullptr;

	if (!bIsValid || bNoneAbilityTag || !bSpecValid) { StatusTag = Tags.Abilities_Status_Locked; }
	else { StatusTag = GetAuraASC()->GetStatusTagFromSpec(*Spec); }

	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StatusTag = StatusTag;

	bool bCanSpendPoints = false;
	bool bCanEquipped = false;
	ShouldEnableButtons(StatusTag, SpellPoints, bCanSpendPoints, bCanEquipped);
	SpellGlobeSelectedDelegate.Broadcast(bCanSpendPoints, bCanEquipped);
}

void USpellMenuWidgetController::SpendPointsButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoints(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendSpell,
                                                     bool& bShouldEnableEquipped)
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	bShouldEnableEquipped = false;
	bShouldEnableSpendSpell = false;

	if (AbilityStatus.MatchesTagExact(Tags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipped = true;
		if (SpellPoints > 0) { bShouldEnableSpendSpell = true; }
	}
	else if (AbilityStatus.MatchesTagExact(Tags.Abilities_Status_Eligible))
	{
		if (SpellPoints > 0) { bShouldEnableSpendSpell = true; }
	}
	else if (AbilityStatus.MatchesTagExact(Tags.Abilities_Status_UnLocked))
	{
		if (SpellPoints > 0) { bShouldEnableSpendSpell = true; }
	}
}
