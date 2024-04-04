// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DataTableTestGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMS_API ADataTableTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADataTableTestGameModeBase();

	virtual void BeginPlay() override; 

	class UDataTable* QuestDataTable;

	class UDataTable* DialogueDataTable;

private:
	

};
