// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverrideNum)
{
	const bool bInServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bInServer) { return; }

	FTransform SpawnTransform;
	FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	SpawnTransform.SetLocation(SocketLocation);
	FRotator SpawnRotation = (TargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		SpawnRotation.Pitch = PitchOverrideNum;
	}
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());

	AAuraProjectile* SpawnProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpawnProjectile->DamageEffectParams = MakeAuraDamageEffectParams();

	SpawnProjectile->FinishSpawning(SpawnTransform);
}
