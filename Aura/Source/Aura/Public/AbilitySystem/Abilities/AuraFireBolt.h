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

	UFUNCTION(BlueprintCallable, Category="FireBolt")
	void SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverrideNum,
	                      AActor* HomingTarget);

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	int32 NumProjectiles = 1;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float HomingAccelerationMin = 1600.f;
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float HomingAccelerationMax = 3200.f;
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	bool bLaunchHomingProjectile = true;

private:
	FString GetFireBoltDescription(int32 Level);
};
