// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectMSGameMode.h"
#include "ProjectMSCharacter.h"
#include "CustomDataTables.h"
#include "UObject/ConstructorHelpers.h"

AProjectMSGameMode::AProjectMSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// by����, �޸𸮿��� �μ��� �Ѿ�� ��θ� �����Ͽ� QuestTable ���������Ʈ ������ �ε��մϴ�.
	static ConstructorHelpers::FObjectFinder<UDataTable> NewDataTable
	(TEXT("/Game/Imported/DataTables/QuestTable"));
	// ���� �ε尡 �����ϸ� QuestDataTable ������ ������ ã�� ������ �ּҸ� �����մϴ�.
	if (NewDataTable.Succeeded())
	{
		QuestDataTable = NewDataTable.Object;
	}

	// by����, �޸𸮿��� �μ��� �Ѿ�� ��θ� �����Ͽ� DialogueTable ���������Ʈ ������ �ε��մϴ�.
	static ConstructorHelpers::FObjectFinder<UDataTable> NewDataTable_01
	(TEXT("/Game/Imported/DataTables/DialogueTable"));
	// ���� �ε尡 �����ϸ� DialogueTable ������ ������ ã�� ������ �ּҸ� �����մϴ�.
	if (NewDataTable_01.Succeeded())
	{
		DialogueDataTable = NewDataTable_01.Object;
	}

}

void AProjectMSGameMode::BeginPlay()
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
}
