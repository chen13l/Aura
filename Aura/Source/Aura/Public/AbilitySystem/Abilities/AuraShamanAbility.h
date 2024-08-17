// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraShamanAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraShamanAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="AuraAbilities|Summon")
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure,Category="AuraAbilities|Summon")
	TSubclassOf<APawn> GetRandomMinionClass();
	
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AuraAbilities|Summon")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AuraAbilities|Summon")
	TArray<TSubclassOf<APawn>> MinionClasses;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AuraAbilities|Summon")
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AuraAbilities|Summon")
	float MaxSpawnDistance = 200.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AuraAbilities|Summon")
	float SpawnSpread = 90.f;
	
};
