// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/CharacterCategoryInfo.h"

FCharacterCategoryDefaultInfo UCharacterCategoryInfo::GetCharacterClassDefaultInfo(ECharacterCatrgory CharacterCategory)
{
	return CharacterCategoryInfo.FindChecked(CharacterCategory);
}
