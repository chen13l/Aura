// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "CharacterCategoryInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterCatrgory : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterCategoryDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Character Default")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Character Default")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Character Default")
	FScalableFloat XPReward = FScalableFloat();
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterCategoryInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Character Defaults")
	TMap<ECharacterCatrgory, FCharacterCategoryDefaultInfo> CharacterCategoryInfo;

	UPROPERTY(EditDefaultsOnly, Category="Common Character Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Character Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditAnywhere, Category="Common Character Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Common Character Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficient;

	FCharacterCategoryDefaultInfo GetCharacterClassDefaultInfo(ECharacterCatrgory CharacterCategory);
};
