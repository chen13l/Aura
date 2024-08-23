// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSpellGlobeSelectedSignature, bool, bSendPointsEnabled, bool, bEquippedEnable, FString,
                                              SpellDescription, FString, SpellNextLevelDescription);

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

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintCallable)
	void SpendPointsButtonPressed();

	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendSpell, bool& bShouldEnableEquipped);

	FSelectedAbility SelectedAbility = {FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoints = 0;
};
