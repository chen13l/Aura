// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FAuraAbilityInfo;
class UAuraAbilitySystemComponent;
class UAbilityInfo;

USTRUCT()
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeDataChangedSignature, float, NewData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitalValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeDataChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeDataChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeDataChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeDataChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Message")
	FMessageWidgetRowSignature MessageWidgetRowSignature;

	UPROPERTY(BlueprintAssignable, Category="GAS|Message")
	FAbilityInfoSignature AbilityInfoDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FOnAttributeDataChangedSignature OnXPPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnplayerStateChangedSignature OnLevelNumChangedDelegate;
	
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	template <typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable,const FGameplayTag& Tag);

	UFUNCTION()
	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC);

	/* for XP, Level */
	void OnXPChanged(int32 NewXP);
	void OnLevelChanged(int32 NewLevel);
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
}
