// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraShamanAbility.h"

TArray<FVector> UAuraShamanAbility::GetSpawnLocations()
{
	const FVector ActorForwardDirection = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector ActorLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	const float DeltaFloat = SpawnSpread / NumMinions;
	const FVector LeftOfSpread = ActorForwardDirection.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);

	TArray<FVector> SpawnLocations;
	for (int32 i = 0; i < NumMinions; ++i)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaFloat * i, FVector::UpVector);
		FVector ChosenSpawnLocation = ActorLocation + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f),
		                                     ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(ChosenSpawnLocation);
	}

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraShamanAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
