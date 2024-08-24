// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelupInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangeDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangeDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnLevelNumChangedDelegate.Broadcast(NewLevel);
		}
	);

	GetAuraASC()->OnEquippedAbilityDelegate.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data)
	                      {
		                      OnHealthChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data)
	                      {
		                      OnMaxHealthChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data)
	                      {
		                      OnManaChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data)
	                      {
		                      OnMaxManaChanged.Broadcast(Data.NewValue);
	                      });


	if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
	{
		if (AuraASC->IsStartupAbilitiesGiven())
		{
			BroadcastAbilityInfo();
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		AuraASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					//"A.1".MatchesTag("A") will return true, "A".MatchesTag("A.1") will return false
					FGameplayTag Message = FGameplayTag::RequestGameplayTag("Message");
					if (Tag.MatchesTag(Message))
					{
						FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowSignature.Broadcast(*Row);
					}
					/*const FString Msg = FString::Printf(TEXT("%s"), *Tag.ToString());
					GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);*/
				}
			}
		);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelupInfo* LevelUpInfo = GetAuraPS()->GetLevelUpInfo();

	const int32 TargetLevel = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInfos.Num() - 1;

	if (MaxLevel >= TargetLevel && TargetLevel > 0)
	{
		const int32 TargetLevelUpRequirement = LevelUpInfo->LevelUpInfos[TargetLevel].XPRequirement;
		const int32 PrevLevelUpRequirement = LevelUpInfo->LevelUpInfos[TargetLevel - 1].XPRequirement;

		const int32 DeltaXP = TargetLevelUpRequirement - PrevLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PrevLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaXP);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel)
{
	OnLevelNumChangedDelegate.Broadcast(GetAuraPS()->GetPlayerLevel());
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& TargetSlotTag,
                                                 const FGameplayTag& PrevSlotTag)
{
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
}
