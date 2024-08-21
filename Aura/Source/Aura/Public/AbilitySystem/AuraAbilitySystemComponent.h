// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, FGameplayTagContainer& /*Asset Tags*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGiven, UAuraAbilitySystemComponent*);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;

	void AbilityActorInfoSet();
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	FORCEINLINE bool IsStartupAbilitiesGiven() const { return bStartupAbilitiesGiven; }
	void ForEachAbility(const FForEachAbility& Delegate);

	FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec);
	FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& Spec);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server,Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
protected:
	bool bStartupAbilitiesGiven = false;

	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec,
	                         FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
