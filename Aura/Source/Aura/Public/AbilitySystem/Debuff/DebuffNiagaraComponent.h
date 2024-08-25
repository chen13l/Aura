// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UDebuffNiagaraComponent();

	void SetDebuffTag(const FGameplayTag& InTag) { DebuffTag = InTag; }

protected:
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;

protected:
	virtual void BeginPlay() override;

	void OnDebuffTagChanged(const FGameplayTag InTag, int32 NewCount);
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
