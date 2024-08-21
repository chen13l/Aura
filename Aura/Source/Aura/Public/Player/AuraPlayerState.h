// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class ULevelupInfo;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChange, int32);

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	ULevelupInfo* GetLevelUpInfo() const;

	FOnPlayerStateChange OnXPChangeDelegate;
	FOnPlayerStateChange OnLevelChangeDelegate;
	FOnPlayerStateChange OnAttributePointsChangeDelegate;
	FOnPlayerStateChange OnSpellPointsChangeDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	void SetLevel(int32 InLevel);
	void AddToLevel(int32 InLevel);

	FORCEINLINE int32 GetXP() const { return XP; }
	void SetXP(int32 InXP);
	void AddToXP(int32 InXP);

	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	void AddToAttributePoinst(int32 InPoints);

	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
	void AddToSpellPoints(int32 InPoints);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(EditDefaultsOnly, Category="AuraPlayerState")
	TObjectPtr<ULevelupInfo> LevelUpInfos;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level, Category="AuraPlayerState")
	int32 Level = 1;
	UFUNCTION()
	void OnRep_Level() const;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP, Category="AuraPlayerState")
	int32 XP = 1;
	UFUNCTION()
	void OnRep_XP() const;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints, Category="AuraPlayerState")
	int32 AttributePoints = 0;
	UFUNCTION()
	void OnRep_AttributePoints() const;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints, Category="AuraPlayerState")
	int32 SpellPoints = 0;
	UFUNCTION()
	void OnRep_SpellPoints() const;
};
