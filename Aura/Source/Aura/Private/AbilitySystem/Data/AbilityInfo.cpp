// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& AbilityTag, bool bLogNotFind)
{
	for (FAuraAbilityInfo Info : AbilityInfos)
	{
		if (Info.AbilityTag.MatchesTagExact(AbilityTag)) { return Info; }
	}

	if (bLogNotFind) { UE_LOG(Log_Aura, Error, TEXT("Can't find info [%s] in AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this)); }

	return FAuraAbilityInfo();
}
