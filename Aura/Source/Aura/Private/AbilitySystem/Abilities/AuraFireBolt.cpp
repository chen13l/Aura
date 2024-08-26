// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

void UAuraFireBolt::SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverrideNum,
                                     AActor* HomingTarget)
{
	const bool bInServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bInServer) { return; }

	FTransform Transform;
	FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	Transform.SetLocation(SocketLocation);
	FRotator Rotation = (TargetLocation - SocketLocation).Rotation();
	if (bOverridePitch) { Rotation.Pitch = PitchOverrideNum; }
	Transform.SetRotation(Rotation.Quaternion());

	const FVector Forward = Rotation.Vector();
	const int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());
	TArray<FRotator> Rotators =
		UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, EffectiveNumProjectiles);

	for (const FRotator& Rot : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraProjectile* SpawnProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		SpawnProjectile->DamageEffectParams = MakeAuraDamageEffectParams();

		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			SpawnProjectile->GetProjectileMovementComponent()->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			SpawnProjectile->HomingTargetComp = NewObject<USceneComponent>(USceneComponent::StaticClass());
			SpawnProjectile->HomingTargetComp->SetWorldLocation(TargetLocation);
			SpawnProjectile->GetProjectileMovementComponent()->HomingTargetComponent = SpawnProjectile->HomingTargetComp;
		}
		SpawnProjectile->SetLifeSpan(5.f);
		SpawnProjectile->GetProjectileMovementComponent()->HomingAccelerationMagnitude = FMath::RandRange(
			HomingAccelerationMin, HomingAccelerationMax);
		SpawnProjectile->GetProjectileMovementComponent()->bIsHomingProjectile = bLaunchHomingProjectile;

		SpawnProjectile->FinishSpawning(SpawnTransform);
	}
}

FString UAuraFireBolt::GetSpellDescription(int32 Level)
{
	bCallGetNextLevelinfo = false;
	return GetFireBoltDescription(Level);
}

FString UAuraFireBolt::GetNextLeveSpellDescription(int32 Level)
{
	bCallGetNextLevelinfo = true;
	return GetFireBoltDescription(Level);
}

FString UAuraFireBolt::GetFireBoltDescription(int32 Level)
{
	AbilityName = FString("Fire Bolt");
	NextLevelString = bCallGetNextLevelinfo ? FString("Next Level:") : FString("Level");
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>%s </>\n\n"

		// Level
		"<Small>%s </><Level>%d</>\n"
		// ManaCost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Number of FireBolts
		"<Default>Launches %d bolts of fire, "
		"exploding on impact and dealing: </>"

		// Damage
		"<Damage>%d</><Default> fire damage with"
		" a chance to burn</>"),

	                       // Values
	                       *AbilityName,
	                       *NextLevelString,
	                       Level,
	                       ManaCost,
	                       Cooldown,
	                       FMath::Min(Level, NumProjectiles),
	                       ScaledDamage);
}
