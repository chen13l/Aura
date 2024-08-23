// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;

	virtual FString GetSpellDescription(int32 Level);
	virtual FString GetNextLeveSpellDescription(int32 Level);
	static FString GetLockedDescription(int32 Level);

	float GetManaCost(float Level) const;
	float GetCooldown(float Level) const;

};
