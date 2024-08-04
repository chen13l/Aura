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

		AAuraProjectile* SpawnProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		UAbilitySystemComponent* SourseASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

		FGameplayEffectContextHandle EffectContextHandle = SourseASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(SpawnProjectile);
		EffectContextHandle.SetAbility(this);
		FHitResult HitResult;
		EffectContextHandle.AddHitResult(HitResult);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(SpawnProjectile);
		EffectContextHandle.AddActors(Actors);

		const FGameplayEffectSpecHandle EffectSpecHandle = SourseASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		
		for(auto& [DamageTag, DamageValue] : DamageTypes)
		{
			float ScaledDamage = DamageValue.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DamageTag, ScaledDamage);
		}

		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

		SpawnProjectile->DamageSpecHandle = EffectSpecHandle;
		SpawnProjectile->SetOwner(GetAvatarActorFromActorInfo());

		SpawnProjectile->FinishSpawning(SpawnTransform);
	}
}
