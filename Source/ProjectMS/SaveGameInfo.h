// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameInfo.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMS_API USaveGameInfo : public USaveGame
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	FString QuestName;
	
	UPROPERTY()
	int32 QuestNumber;
	
	UPROPERTY()
	int32 ObjectiveItem_Number;
	
	UPROPERTY()
	int32 ObjectiveItem_Count;
	
	UPROPERTY()
	int32 RewardItem_Number;

};
