// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag;
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag;
	//for update cooldown time in spell bar
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag;
	//for select in spell menu
	UPROPERTY( BlueprintReadOnly)
	FGameplayTag StatusTag;
	//for equip spell
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AbilityType;
	//for update spell menu image
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> IconTexture = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UMaterialInterface> BgMaterial = nullptr;
	//for spend spell points 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1;
	//for GiveAbility() to actor when unlocked ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;
};

/**
 * 
 */
UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AbilityInformation")
	TArray<FAuraAbilityInfo> AbilityInfos;

	FAuraAbilityInfo FindAbilityInfoByTag(const FGameplayTag& Tag, bool bLogNotFind = false);
};
