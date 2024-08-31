// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"


void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		FGameplayAbilitySpec GameplayAbilitySpec(AbilityClass, 1);
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(GameplayAbilitySpec.Ability))
		{
			GameplayAbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GameplayAbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(GameplayAbilitySpec);
			//GiveAbilityAndActivateOnce(GameplayAbilitySpec);	
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : PassiveAbilities)
	{
		FGameplayAbilitySpec GameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(GameplayAbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) { return; }
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTag(InputTag))
		{
			AbilitySpecInputPressed(Spec);
			if (Spec.IsActive())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
				                      Spec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) { return; }
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) { return; }
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle,
			                      AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(Spec))
		{
			UE_LOG(Log_Aura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for (FGameplayTag Tag : Spec.Ability.Get()->AbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for (FGameplayTag Tag : Spec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for (FGameplayTag StatusTag : Spec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagByAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetAbilitySpecFromTag(AbilityTag))
	{
		return GetStatusTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagByAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetAbilitySpecFromTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::IsSlotEmpty(const FGameplayTag& SlotTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(SlotTag)) { return false; }
	}
	return true;
}

bool UAuraAbilitySystemComponent::IsAbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& SlotTag)
{
	return Spec.DynamicAbilityTags.HasTagExact(SlotTag);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecBySlot(const FGameplayTag& SlotTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(SlotTag)) { return &AbilitySpec; }
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoByTag(GetAbilityTagFromSpec(AbilitySpec));
	return Info.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& SlotTag)
{
	ClearSlot(Spec);
	Spec.DynamicAbilityTags.AddTag(SlotTag);
}

bool UAuraAbilitySystemComponent::IsAbilityHasAnySlotTag(const FGameplayAbilitySpec& Spec)
{
	return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetAbilitySpecFromTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : Spec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTagExact(AbilityTag)) { return &Spec; }
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecFromTag(AbilityTag))
	{
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& StatusTag = GetStatusTagFromSpec(*AbilitySpec);

		const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
		const bool bStatusValid = StatusTag.MatchesTagExact(Tags.Abilities_Status_UnLocked) || StatusTag.MatchesTagExact(
			Tags.Abilities_Status_Equipped);
		if (bStatusValid)
		{
			//Handle activation/deactivation ability
			if (!IsSlotEmpty(SlotTag))
			{
				// if selected ability the same as selected slot's ability, return 
				FGameplayAbilitySpec* SpecInSlot = GetSpecBySlot(SlotTag);
				if (AbilityTag.MatchesTagExact(GetInputTagFromSpec(*SpecInSlot)))
				{
					ClientEquippedAbility(AbilityTag, Tags.Abilities_Status_Equipped, SlotTag, PrevSlot);
					return;
				}

				if (IsPassiveAbility(*SpecInSlot)) { OnDeActivatePassiveAbilityDelegate.Broadcast(GetAbilityTagFromSpec(*SpecInSlot)); }
				ClearSlot(*SpecInSlot);
			}
			if (!IsAbilityHasAnySlotTag(*AbilitySpec))
			{
				if (IsPassiveAbility(*AbilitySpec)) { TryActivateAbility(AbilitySpec->Handle); }
			}
			AssignSlotToAbility(*AbilitySpec,SlotTag);
			
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		ClientEquippedAbility(AbilityTag, Tags.Abilities_Status_Equipped, SlotTag, PrevSlot);
	}
}

void UAuraAbilitySystemComponent::ClientEquippedAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
                                                                       const FGameplayTag& NewSlotTag, const FGameplayTag& PrevSlotTag)
{
	OnEquippedAbilityDelegate.Broadcast(AbilityTag, StatusTag, NewSlotTag, PrevSlotTag);
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec& AbilitySpec)
{
	const FGameplayTag SlotTag = GetInputTagFromSpec(AbilitySpec);
	AbilitySpec.DynamicAbilityTags.RemoveTag(SlotTag);
	MarkAbilitySpecDirty(AbilitySpec);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& SlotTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&Spec, SlotTag))
		{
			ClearSlot(Spec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& SlotTag)
{
	for (FGameplayTag Tag : AbilitySpec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(SlotTag))
		{
			return true;
		}
	}
	return false;
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)
{
	if (UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInfos)
		{
			if (Level < Info.LevelRequirement || !Info.AbilityTag.IsValid()) { continue; }
			if (GetAbilitySpecFromTag(Info.AbilityTag) == nullptr)
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
				AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
				GiveAbility(AbilitySpec);
				//force replicate
				MarkAbilitySpecDirty(AbilitySpec);
				ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
			}
		}
	}
}

bool UAuraAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& Description, FString& NextLevelDescription)
{
	if (const FGameplayAbilitySpec* Spec = GetAbilitySpecFromTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(Spec->Ability))
		{
			Description = AuraAbility->GetSpellDescription(AuraAbility->GetAbilityLevel());
			NextLevelDescription = AuraAbility->GetNextLeveSpellDescription(AuraAbility->GetAbilityLevel() + 1);
			return true;
		}
	}
	UAbilityInfo* Info = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (Info)
	{
		if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
		{
			Description = FString();
		}
		else
		{
			Description = UAuraGameplayAbility::GetLockedDescription(Info->FindAbilityInfoByTag(AbilityTag).LevelRequirement);
		}
		NextLevelDescription = FString();
	}

	return false;
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = GetAbilitySpecFromTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		FGameplayTag StatusTag = GetStatusTagFromSpec(*Spec);
		if (GetStatusTagFromSpec(*Spec) == Tags.Abilities_Status_Eligible)
		{
			Spec->DynamicAbilityTags.RemoveTag(Tags.Abilities_Status_Eligible);
			Spec->DynamicAbilityTags.AddTag(Tags.Abilities_Status_UnLocked);
			StatusTag = Tags.Abilities_Status_UnLocked;
		}
		else if (GetStatusTagFromSpec(*Spec) == Tags.Abilities_Status_Equipped || GetStatusTagFromSpec(*Spec) == Tags.Abilities_Status_UnLocked)
		{
			Spec->Level += 1;
		}
		ClientUpdateAbilityStatus(AbilityTag, StatusTag, Spec->Level);
		MarkAbilitySpecDirty(*Spec);
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
                                                                           int32 AbilityLevel)
{
	OnAbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC,
                                                                     const FGameplayEffectSpec& GameplayEffectSpec,
                                                                     FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	GameplayEffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
