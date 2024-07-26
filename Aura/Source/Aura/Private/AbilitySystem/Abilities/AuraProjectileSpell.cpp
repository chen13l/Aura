// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	const bool bInServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bInServer) { return; }

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());

	if (CombatInterface)
	{
		FTransform SpawnTransform;
		FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		SpawnTransform.SetLocation(SocketLocation);
		FRotator SpawnRotation = (TargetLocation - SocketLocation).Rotation();
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());

		AAuraProjectile* SpawnActorDeferred = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		UAbilitySystemComponent* SourseASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle EffectSpecHandle = SourseASC->MakeOutgoingSpec(
			DamageEffectClass, GetAbilityLevel(), SourseASC->MakeEffectContext());

		float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());

		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, GameplayTags.Damage,
		                                                              ScaledDamage);

		SpawnActorDeferred->DamageSpecHandle = EffectSpecHandle;
		SpawnActorDeferred->SetOwner(GetAvatarActorFromActorInfo());

		SpawnActorDeferred->FinishSpawning(SpawnTransform);
	}
}
