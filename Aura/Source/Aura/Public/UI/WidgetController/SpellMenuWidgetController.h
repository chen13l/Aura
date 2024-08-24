// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSpellGlobeSelectedSignature, bool, bSendPointsEnabled, bool, bEquippedEnable, FString,
                                              SpellDescription, FString, SpellNextLevelDescription);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaitForEquippedSpellSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellGlobeReassignSignature, const FGameplayTag&, AbilityTag);

struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag StatusTag = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnplayerStateChangedSignature OnSpellPointsChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnWaitForEquippedSpellSignature WaitForEquippedSpellDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnWaitForEquippedSpellSignature StopWaitingEquippedSpellDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnSpellGlobeReassignSignature SpellGlobeReassignDelegate;
	
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintCallable)
	void SpellGlobeDeselected();
	UFUNCTION(BlueprintCallable)
	void SpendPointsButtonPressed();
	UFUNCTION(BlueprintCallable)
	void EquippedButtonPressed();
	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& TargetSlotTag, const FGameplayTag& AbilityType);

	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendSpell, bool& bShouldEnableEquipped);

	FSelectedAbility SelectedAbility = {FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoints = 0;
	bool bWaitingForEquippedButton = false;

	//cache ability slot if had equipped
	FGameplayTag SelectedSlot;

protected:
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& TargetSlotTag,
	                       const FGameplayTag& PrevSlotTag);
};
