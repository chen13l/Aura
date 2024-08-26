// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, FGameplayTagContainer& /*Asset Tags*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAbilityStatusChangedSignature, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/,
                                       int32 /*AbilityLevel*/)
DECLARE_MULTICAST_DELEGATE_FourParams(FOnEquippedAbilitySignature, const FGameplayTag&/*AbilityTag*/, const FGameplayTag&/*StatusTag*/,
                                      const FGameplayTag&/*NewSlotTag*/, const FGameplayTag&/*PrevSlotTag*/)
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
	FOnAbilityStatusChangedSignature OnAbilityStatusChangedDelegate;
	FOnEquippedAbilitySignature OnEquippedAbilityDelegate;

	void AbilityActorInfoSet();
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	FORCEINLINE bool IsStartupAbilitiesGiven() const { return bStartupAbilitiesGiven; }
	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& Spec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& Spec);
	FGameplayTag GetStatusTagByAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagByAbilityTag(const FGameplayTag& AbilityTag);

	FGameplayAbilitySpec* GetAbilitySpecFromTag(const FGameplayTag& AbilityTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag);
	UFUNCTION(Client, Reliable)
	void ClientEquippedAbility(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& NewSlotTag,
	                           const FGameplayTag& PrevSlotTag);
	void ClearSlot(FGameplayAbilitySpec& AbilitySpec);
	void ClearAbilitiesOfSlot(const FGameplayTag& SlotTag);
	static bool AbilityHasSlot(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& SlotTag);

	void UpdateAbilityStatus(int32 Level);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);

	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& Description, FString& NextLevelDescription);

protected:
	bool bStartupAbilitiesGiven = false;

	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec,
	                         FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);
};
