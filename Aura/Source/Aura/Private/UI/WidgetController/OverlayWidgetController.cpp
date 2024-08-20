// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelupInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitalValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnXPChangeDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->OnLevelChangeDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnLevelNumChangedDelegate.Broadcast(NewLevel);
		}
	);

	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data)
	                      {
		                      OnHealthChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data)
	                      {
		                      OnMaxHealthChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data)
	                      {
		                      OnManaChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data)
	                      {
		                      OnMaxManaChanged.Broadcast(Data.NewValue);
	                      });


	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (AuraASC->IsStartupAbilitiesGiven())
		{
			OnInitializeStartupAbilities(AuraASC);
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
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

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC)
{
	if (!AuraASC->IsStartupAbilitiesGiven()) { return; }
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this,AuraASC](const FGameplayAbilitySpec& Spec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AuraASC->GetAbilityTagFromSpec(Spec));
		Info.InputTag = AuraASC->GetInputTagFromSpec(Spec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	AuraASC->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelupInfo* LevelUpInfo = AuraPlayerState->GetLevelUpInfo();

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
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnLevelNumChangedDelegate.Broadcast(AuraPlayerState->GetPlayerLevel());
}
