// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelupInfo.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

ULevelupInfo* AAuraPlayerState::GetLevelUpInfo() const
{
	return LevelUpInfos;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

void AAuraPlayerState::OnRep_Level() const
{
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_XP() const
{
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints() const
{
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddToAttributePoinst(int32 InPoints)
{
	AttributePoints += InPoints;
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints() const
{
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 InPoints)
{
	SpellPoints += InPoints;
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
}
