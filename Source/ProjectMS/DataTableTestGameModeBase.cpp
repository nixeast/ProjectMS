// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableTestGameModeBase.h"
#include "CustomDataTables.h"
#include "UObject/ConstructorHelpers.h"

ADataTableTestGameModeBase::ADataTableTestGameModeBase()
{


	static ConstructorHelpers::FObjectFinder<UDataTable> NewDataTable(TEXT("/Game/Imported/DataTables/QuestTable"));

	if (NewDataTable.Succeeded())
	{ 
		QuestDataTable = NewDataTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> NewDataTable_01(TEXT("/Game/Imported/DataTables/DialogueTable"));

	if (NewDataTable_01.Succeeded())
	{
		DialogueDataTable = NewDataTable_01.Object;
	}
}

void ADataTableTestGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (QuestDataTable != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load quest data table succeed.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load quest data table failed.."));
	}

	if (DialogueDataTable != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load dialogue data table succeed.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load dialogue data table failed.."));
	}

	/*if (QuestDataTable != nullptr)
	{ 
		for (int32 i = 1; i <= 5; i++) 
		{ 
			FQuestTableRow* QuestTableRow = QuestDataTable->FindRow<FQuestTableRow>(FName(*(FString::FormatAsNumber(i))), FString(""));
		
			UE_LOG(LogTemp, Log, TEXT("Number%d: QuestNumber(%d) QuestSuccess(%s)"), i, QuestTableRow->QuestNumber, QuestTableRow->QuestSuccess? TEXT("True") : TEXT("false"));
		} 
	}*/

}
