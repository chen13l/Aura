// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelupInfo.h"

int32 ULevelupInfo::FindLevelForXP(const int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		//arrive max level
		if (LevelUpInfos.Num() - 1 <= Level) { return Level; }

		if (XP >= LevelUpInfos[Level].XPRequirement) { ++Level; }
		else { bSearching = false; }
	}
	return Level;
}
