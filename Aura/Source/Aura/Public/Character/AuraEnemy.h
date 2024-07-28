// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterCategoryInfo.h"
#include "Character/CharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
/**
 *
 */
UCLASS()
class AURA_API AAuraEnemy : public ACharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	/*  EnemyInterface  */
	virtual void HighLight() override;
	virtual void UnHighLight() override;
	/*  End EnemyInterface  */

	/*Combat Interface*/
	virtual int32 GetPlayerLevel() const override;
	/*End Combat Interface*/

	UPROPERTY(BlueprintAssignable)
	FOnAttributeDataChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeDataChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 250.f;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorinfo() override;
	virtual void InitDefaultAttributeByGE() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterCatrgory CharacterCatrgory = ECharacterCatrgory::Warrior;
};
