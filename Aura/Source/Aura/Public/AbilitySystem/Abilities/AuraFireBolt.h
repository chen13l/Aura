// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
protected:
	
	virtual FString GetSpellDescription(int32 Level) override;
	virtual FString GetNextLeveSpellDescription(int32 Level) override;

	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 1;

private:
	FString GetFireBoltDescription(int32 Level);
};
