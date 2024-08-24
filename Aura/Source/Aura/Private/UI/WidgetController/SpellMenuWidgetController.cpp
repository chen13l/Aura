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
			FString Description;
			FString NextLevelDescription;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bCanSpendPoints, bCanEquipped);
			GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bCanSpendPoints, bCanEquipped, Description, NextLevelDescription);
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
		FString Description;
		FString NextLevelDescription;
		ShouldEnableButtons(SelectedAbility.StatusTag, CurrentSpellPoints, bCanSpendPoints, bCanEquipped);
		SpellGlobeSelectedDelegate.Broadcast(bCanSpendPoints, bCanEquipped, Description, NextLevelDescription);
	});

	GetAuraASC()->OnEquippedAbilityDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquippedButton)
	{
		const FGameplayTag& AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
		StopWaitingEquippedSpellDelegate.Broadcast(AbilityType);
		bWaitingForEquippedButton = false;
	}
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
	FString Description;
	FString NextLevelDescription;
	ShouldEnableButtons(StatusTag, SpellPoints, bCanSpendPoints, bCanEquipped);
	GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bCanSpendPoints, bCanEquipped, Description, NextLevelDescription);
}	

void USpellMenuWidgetController::SpellGlobeDeselected()
{
	if (bWaitingForEquippedButton)
	{
		const FGameplayTag& AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
		StopWaitingEquippedSpellDelegate.Broadcast(AbilityType);
		bWaitingForEquippedButton = false;
	}
	FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	SelectedAbility.AbilityTag = Tags.Abilities_None;
	SelectedAbility.StatusTag = Tags.Abilities_Status_Locked;

	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::SpendPointsButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoints(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::EquippedButtonPressed()
{
	const FGameplayTag& AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
	WaitForEquippedSpellDelegate.Broadcast(AbilityType);
	bWaitingForEquippedButton = true;

	const FGameplayTag& StatusTag = GetAuraASC()->GetStatusTagByAbilityTag(SelectedAbility.AbilityTag);
	if (StatusTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagByAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& TargetSlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquippedButton) { return; }
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) { return; }

	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag, TargetSlotTag);
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
		bShouldEnableEquipped = true;
		if (SpellPoints > 0) { bShouldEnableSpendSpell = true; }
	}
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& TargetSlotTag,
                                                   const FGameplayTag& PrevSlotTag)
{
	bWaitingForEquippedButton = false;

	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	//for clear slot target that has been before
	FAuraAbilityInfo ClearLastSlotInfo;
	ClearLastSlotInfo.StatusTag = Tags.Abilities_Status_UnLocked;
	ClearLastSlotInfo.InputTag = PrevSlotTag;
	ClearLastSlotInfo.AbilityTag = Tags.Abilities_None;
	AbilityInfoDelegate.Broadcast(ClearLastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = TargetSlotTag;
	AbilityInfoDelegate.Broadcast(Info);

	StopWaitingEquippedSpellDelegate.Broadcast(Info.AbilityType);
	SpellGlobeReassignDelegate.Broadcast(AbilityTag);
	SpellGlobeDeselected();
}
