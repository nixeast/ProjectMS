// Fill out your copyright notice in the Description page of Project Settings.


#include "InterMissionUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "SaveGameInfo.h"
#include "DataTableTestGameModeBase.h"
#include "CustomDataTables.h"
#include "Engine/DataTable.h"
#include "Components/TextBlock.h"
#include "InGameManager.h"

void UInterMissionUI::NativeOnInitialized()
{
	InitQuestBoardButtons();

	BindWidget_InterMission();

	ShowMainScreen();

	LoadCurrentQuest();

}

void UInterMissionUI::GotoIngame()
{
	UGameplayStatics::OpenLevel(this, FName("Stage01"));
	UE_LOG(LogTemp, Display, TEXT("GotoIngame()"));
}

void UInterMissionUI::ShowQuestBoard()
{
	CanvasPanel_InterMissionMain->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_IntermissionQuest->SetVisibility(ESlateVisibility::Visible);
	
	ShowQuestData();
}

void UInterMissionUI::ShowMainScreen()
{
	CanvasPanel_InterMissionMain->SetVisibility(ESlateVisibility::Visible);
	CanvasPanel_IntermissionQuest->SetVisibility(ESlateVisibility::Hidden);
}

void UInterMissionUI::SaveCurrentQuest()
{
	USaveGameInfo* SaveDataPtr = NewObject<USaveGameInfo>();
	SaveDataPtr->QuestName=TEXT("TestQuest");
	SaveDataPtr->QuestNumber = SelectedQuestButtonNum;
	SaveDataPtr->ObjectiveItem_Number=1;
	SaveDataPtr->ObjectiveItem_Count=2;
	SaveDataPtr->RewardItem_Number=3;
	
	bool SaveResult = UGameplayStatics::SaveGameToSlot(SaveDataPtr, SaveSlotName, 0);
	
	if (false == SaveResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("Save failed.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save successed.."));
		UE_LOG(LogTemp, Display, TEXT("SaveDataPtr->QuestNumber: %s"), *SaveDataPtr->QuestName);
		UE_LOG(LogTemp, Display, TEXT("SaveDataPtr->QuestNumber: %d"), SaveDataPtr->QuestNumber);
	}
}

void UInterMissionUI::LoadCurrentQuest()
{
	USaveGameInfo* SaveDataPtr = Cast<USaveGameInfo>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

	if (SaveDataPtr == nullptr)
	{
		SaveDataPtr = GetMutableDefault<USaveGameInfo>();
		UE_LOG(LogTemp, Warning, TEXT("GetMutableDefault.."));
	}
	else
	{
		
		UE_LOG(LogTemp, Warning, TEXT("Load Succeed.."));
		UE_LOG(LogTemp, Display, TEXT("Last saved QuestName:%s"), *SaveDataPtr->QuestName);
	}
}

void UInterMissionUI::ShowQuestData()
{
	CurrentGameModeBase = Cast<ADataTableTestGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (CurrentGameModeBase != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load CurrentGameModeBase succeed.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load CurrentGameModeBase failed.."));
	}

	CurrentQuestDataTable = CurrentGameModeBase->QuestDataTable;

	if (CurrentQuestDataTable != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load quest data table succeed.."));

		/*for (int32 i = 1; i <= 5; i++)
		{
			FQuestTableRow* QuestTableRow = CurrentQuestDataTable->FindRow<FQuestTableRow>(FName(*(FString::FormatAsNumber(i))), FString(""));

			UE_LOG(LogTemp, Log, TEXT("Number%d: Quest_Order(%d) Quest_Name(%s)"), i, QuestTableRow->Quest_Order, *QuestTableRow->Quest_Name);
			
			FString TempString = FString::Printf(TEXT("Number%d: Quest_Order(%d) Quest_Name(%s)"), i, QuestTableRow->Quest_Order, *QuestTableRow->Quest_Name);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TempString);
			}
		}*/
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load quest data table failed.."));
	}
}

void UInterMissionUI::SelectQuestContent_Slot01()
{
	SelectedQuestButtonNum = 1;
	ShowSelectedQuestInfo(SelectedQuestButtonNum);
}

void UInterMissionUI::SelectQuestContent_Slot02()
{
	SelectedQuestButtonNum = 2;
	ShowSelectedQuestInfo(SelectedQuestButtonNum);
}

void UInterMissionUI::SelectQuestContent_Slot03()
{
	SelectedQuestButtonNum = 3;
	ShowSelectedQuestInfo(SelectedQuestButtonNum);
}

void UInterMissionUI::SelectQuestContent_Slot04()
{
	SelectedQuestButtonNum = 4;
	ShowSelectedQuestInfo(SelectedQuestButtonNum);
}

void UInterMissionUI::SelectQuestContent_Slot05()
{
	SelectedQuestButtonNum = 5;
	ShowSelectedQuestInfo(SelectedQuestButtonNum);
}

void UInterMissionUI::ShowSelectedQuestInfo(int32 Number)
{
	FQuestTableRow* QuestTableRow = CurrentQuestDataTable->FindRow<FQuestTableRow>(FName(*(FString::FormatAsNumber(Number))), FString(""));

	TextBlock_QuestName->SetText(FText::FromString(QuestTableRow->Quest_Name));
	TextBlock_ClientName->SetText(FText::FromString(QuestTableRow->Quest_ClientName));
	TextBlock_QuestStory->SetText(FText::FromString(QuestTableRow->Quest_Content));
	TextBlock_ObjectiveName->SetText(FText::FromString(QuestTableRow->Objective_Name));
	TextBlock_ObjectiveCount->SetText(FText::AsNumber(QuestTableRow->Objective_Count));
	TextBlock_RewardName->SetText(FText::FromString(QuestTableRow->Reward_Name));
	TextBlock_RewardCount->SetText(FText::AsNumber(QuestTableRow->Reward_Count));
}

void UInterMissionUI::InitQuestBoardButtons()
{
	btn_QuestBoard.Init(nullptr, 5);
}

void UInterMissionUI::BindWidget_InterMission()
{
	CanvasPanel_InterMissionMain = Cast<UCanvasPanel>(this->GetWidgetFromName("CanvasPanel_InterMissionMain"));
	CanvasPanel_IntermissionQuest = Cast<UCanvasPanel>(this->GetWidgetFromName("CanvasPanel_IntermissionQuest"));

	btn_GameStart = Cast<UButton>(GetWidgetFromName("btn_GameStart"));
	btn_GameStart->OnClicked.AddDynamic(this, &UInterMissionUI::GotoIngame);

	btn_Quest = Cast<UButton>(this->GetWidgetFromName("btn_Quest"));
	btn_Quest->OnClicked.AddDynamic(this, &UInterMissionUI::ShowQuestBoard);

	btn_Back = Cast<UButton>(this->GetWidgetFromName("btn_Back"));
	btn_Back->OnClicked.AddDynamic(this, &UInterMissionUI::ShowMainScreen);

	btn_QuestConfirm = Cast<UButton>(this->GetWidgetFromName("btn_QuestConfirm"));
	btn_QuestConfirm->OnClicked.AddDynamic(this, &UInterMissionUI::SaveCurrentQuest);
	
	TextBlock_QuestName = Cast<UTextBlock>(this->GetWidgetFromName("TextBlock_QuestName"));
	TextBlock_ClientName = Cast<UTextBlock>(this->GetWidgetFromName("TextBlock_ClientName"));
	TextBlock_QuestStory = Cast<UTextBlock>(this->GetWidgetFromName("TextBlock_QuestStory"));
	TextBlock_ObjectiveName = Cast<UTextBlock>(this->GetWidgetFromName("TextBlock_ObjectiveName"));
	TextBlock_ObjectiveCount = Cast<UTextBlock>(this->GetWidgetFromName("TextBlock_ObjectiveCount"));
	TextBlock_RewardName = Cast<UTextBlock>(this->GetWidgetFromName("TextBlock_RewardName"));
	TextBlock_RewardCount = Cast<UTextBlock>(this->GetWidgetFromName("TextBlock_RewardCount"));

	for (int32 i = 0; i < 5; i++)
	{
		FString Temp = FString(TEXT("btn_QuestBoard_")) + FString::FromInt(i);
		
		btn_QuestBoard[i] = Cast<UButton>(GetWidgetFromName(FName(*Temp)));
		FString TempString = FString::FromInt(i);
		//btn_QuestBoard[i]->SetDisplayLabel(TempString);

		if (btn_QuestBoard[i] != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Bind %s succeed.."), *btn_QuestBoard[i]->GetLabelText().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Bind btn_QuestBoad%d failed.."), i);
		}

	}

	btn_QuestBoard[0]->OnClicked.AddDynamic(this, &UInterMissionUI::SelectQuestContent_Slot01);
	btn_QuestBoard[1]->OnClicked.AddDynamic(this, &UInterMissionUI::SelectQuestContent_Slot02);
	btn_QuestBoard[2]->OnClicked.AddDynamic(this, &UInterMissionUI::SelectQuestContent_Slot03);
	btn_QuestBoard[3]->OnClicked.AddDynamic(this, &UInterMissionUI::SelectQuestContent_Slot04);
	btn_QuestBoard[4]->OnClicked.AddDynamic(this, &UInterMissionUI::SelectQuestContent_Slot05);
}