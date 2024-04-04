// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectMSGameMode.generated.h"

UCLASS(minimalapi)
class AProjectMSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectMSGameMode();

	virtual void BeginPlay() override;

	class UDataTable* QuestDataTable;
	class UDataTable* DialogueDataTable;
};



