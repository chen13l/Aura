// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorinfo() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Class Defaults", meta=(AllowPrivateAccess))
	int32 Level = 1;
};
