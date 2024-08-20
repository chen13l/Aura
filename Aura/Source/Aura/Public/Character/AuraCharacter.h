// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class AAuraPlayerState;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public ACharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/*Combat Interface*/
	virtual int32 GetPlayerLevel_Implementation() const override;
	/*End Combat Interface*/

	/* Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 GetLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 InLevel) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InPoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InPoints) override;
	/* End Player Interface */
protected:

private:
	virtual void InitAbilityActorinfo() override;
};
