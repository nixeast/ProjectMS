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

	// by정훈, 메모리에서 인수로 넘어온 경로를 참조하여 QuestTable 스프레드시트 에셋을 로드합니다.
	static ConstructorHelpers::FObjectFinder<UDataTable> NewDataTable
	(TEXT("/Game/Imported/DataTables/QuestTable"));
	// 에셋 로드가 성공하면 QuestDataTable 포인터 변수에 찾은 에셋의 주소를 저장합니다.
	if (NewDataTable.Succeeded())
	{
		QuestDataTable = NewDataTable.Object;
	}

	// by정훈, 메모리에서 인수로 넘어온 경로를 참조하여 DialogueTable 스프레드시트 에셋을 로드합니다.
	static ConstructorHelpers::FObjectFinder<UDataTable> NewDataTable_01
	(TEXT("/Game/Imported/DataTables/DialogueTable"));
	// 에셋 로드가 성공하면 DialogueTable 포인터 변수에 찾은 에셋의 주소를 저장합니다.
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
