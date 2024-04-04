// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameManager.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.h"
#include "Components/TextBlock.h"
#include "ProjectMSCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/GridPanel.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "InventoryItemButton.h"
#include "SaveGameInfo.h"
#include "Kismet/GameplayStatics.h"
#include "DataTableTestGameModeBase.h"
#include "Engine/DataTable.h"
#include "ProjectMSGameMode.h"
#include "Components/SceneCaptureComponent2D.h"
#include "OverlayActor.h"
#include "Components/Image.h"
#include "Item.h"


// Sets default values
AInGameManager::AInGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInGameManager::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AProjectMSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	PlayerCharacter->FindCurrentInGameManager();

	InitInventorySlotArray();

	UE_LOG(LogTemp, Display, TEXT("CurrentInventorySlotArray Num: %d"), CurrentInventorySlotArray.Num());

	CreateInGameUI();
	LoadSavedData();
	SetCurrentGameModeBase();
	SetCurrentQuestDataTable();
	SetCurrentDialogueDataTable();

	LoadQuestDataFromTable(CurrentQuestNumber);
	IsCompleteQuest = false;
	BindResultPanelWidgets();
	SetResultPanelWidgets();
	
	CanvasPanel_Result->SetVisibility(ESlateVisibility::Hidden);
	Panel_ItemGet->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_ItemInfo->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_GameOver->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_Interaction->SetVisibility(ESlateVisibility::Hidden);
}

void AInGameManager::InitInventorySlotArray()
{
	CurrentInventorySlotArray.Init(nullptr, 9);
}

int32 AInGameManager::CheckEmptyInventorySlotNum()
{
	for (int32 i = 0; i < CurrentInventorySlotArray.Num(); i++)
	{
		

		if (CurrentInventorySlotArray[i] == nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("EmptySlot is:%d.."), i);

			SlotNumToStartAdd = i;

			return i;
		}
	}

	return -1;
}

void AInGameManager::InitPlayerGunAmmo()
{
	if (InGameUI != nullptr)
	{
		sting_LoadedAmmo = FString::FromInt(PlayerCharacter->GetLoadedAmmo());
		sting_MagazineCapacity = FString::FromInt(PlayerCharacter->GetMagazineCapacity());
		sting_LeftAmmo = FString::FromInt(PlayerCharacter->GetLeftAmmo());

		text_LoadedAmmo->SetText(FText::FromString(sting_LoadedAmmo));
		text_MagazineCapacity->SetText(FText::FromString(sting_MagazineCapacity));
		text_LeftAmmo->SetText(FText::FromString(sting_LeftAmmo));
	}
}

void AInGameManager::BindWidget()
{
	if (InGameUI != nullptr)
	{
		BindWidget_AimHUD();
		BindWidget_ItemGetPopup();
		BindWidget_ItemHarvesting();
		BindWidget_ItemPickupWindow();
		BindWidget_Inventory();		
		BindWidget_GameOver();
		BindWidget_StageQuest();
		BindWidget_Interaction();
	}
}

void AInGameManager::BindWidget_AimHUD()
{
	Panel_AimMode = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_AimMode"));
	UProgressBar_PlayerHealth = Cast<UProgressBar>(InGameUI->GetWidgetFromName("ProgressBar_PlayerHealth"));

	text_LoadedAmmo = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_LoadedAmmo"));
	text_MagazineCapacity = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_MagazineCapacity"));
	text_LeftAmmo = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_LeftAmmo"));
}

void AInGameManager::BindWidget_ItemHarvesting()
{
	Panel_HarvestMode = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_HarvestMode"));
	UProgressBar_Harvest = Cast<UProgressBar>(InGameUI->GetWidgetFromName("ProgressBar_Harvest"));
}

void AInGameManager::BindWidget_ItemPickupWindow()
{
	CanvasPanel_ItemInfo = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_ItemInfo"));
	Button_ItemInfo_Confirm = Cast<UButton>(InGameUI->GetWidgetFromName(TEXT("Button_ItemInfo_Confirm")));
	Button_ItemInfo_Confirm->OnClicked.AddDynamic(this, &AInGameManager::HideUI_GetItemInfo);
	Image_ItemInfo = Cast<UImage>(InGameUI->GetWidgetFromName(TEXT("Image_ItemInfo")));
}

void AInGameManager::BindWidget_ItemGetPopup()
{
	Panel_ItemGet = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_ItemGet"));
	TextBlock_GetItemName = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_GetItemName"));
}

void AInGameManager::BindWidget_Inventory()
{
	Panel_InventoryMode = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_Inventory"));

	TextBlock_CurrentItemName = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_CurrentItemName"));
	TextBlock_CurrentItemCount = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_CurrentItemCount"));
	TextBlock_CurrentItemInfo = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_CurrentItemInfo"));

	Button_UseItem = Cast<UButton>(InGameUI->GetWidgetFromName(TEXT("Button_UseItem")));
	Button_UseItem->OnClicked.AddDynamic(this, &AInGameManager::UseSelectedItem);

	Button_InventoryClose = Cast<UButton>(InGameUI->GetWidgetFromName(TEXT("Button_InventoryClose")));
	Button_InventoryClose->OnClicked.AddDynamic(this, &AInGameManager::CloseInventoryWindow);

	Panel_InventorySlotPanel = Cast<UGridPanel>(InGameUI->GetWidgetFromName("GridPanel_InventorySlots"));
}

void AInGameManager::BindWidget_Interaction()
{
	CanvasPanel_Interaction = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_Interaction"));
	
	Button_Interaction_yes = Cast<UButton>(InGameUI->GetWidgetFromName(TEXT("Button_Interaction_yes")));
	Button_Interaction_yes->OnClicked.AddDynamic(this, &AInGameManager::ShowNextUI_Interaction);
	
	Button_Interaction_no = Cast<UButton>(InGameUI->GetWidgetFromName(TEXT("Button_Interaction_no")));
	Button_Interaction_no->OnClicked.AddDynamic(this, &AInGameManager::HideUI_Interaction);
	
	Text_Interact_Description = Cast<UTextBlock>(InGameUI->GetWidgetFromName("Text_Interact_Description"));
}

void AInGameManager::BindWidget_StageQuest()
{
	TextBlock_StageQuestName = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_StageQuestName"));
	TextBlock_StageQuestContent = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_StageQuestContent"));
	TextBlock_StageQuestObjective = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_StageQuestObjective"));
	TextBlock_StageQuestReward = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_StageQuestReward"));
	TextBlock_StageQuestCurrent = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_StageQuestCurrent"));

	TextBlock_StageQuestObjectiveCount = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_StageQuestObjectiveCount"));
	TextBlock_StageQuestCurrentCount = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_StageQuestCurrentCount"));
	TextBlock_StageQuestPercent = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_StageQuestPercent"));
}

void AInGameManager::BindWidget_GameOver()
{
	// by����, ���ӿ��� �г��� �ּҸ� �����մϴ�.
	CanvasPanel_GameOver = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_GameOver"));
	// by����, ���ӿ��� �г��� Ÿ��Ʋȭ�� ��ư �ּҸ� �����մϴ�.
	Button_GotoTitle = Cast<UButton>(InGameUI->GetWidgetFromName(TEXT("Button_GotoTitle")));
	// by����, ���ӿ��� �г��� Ÿ��Ʋȭ�� ��ư�� Ŭ���̺�Ʈ�� �Լ��� ���ε��մϴ�.
	Button_GotoTitle->OnClicked.AddDynamic(this, &AInGameManager::GotoInterMission);
}

void AInGameManager::ShowInGameUI()
{
	if (InGameUI != nullptr)
	{
		InGameUI->SetVisibility(ESlateVisibility::Visible);

		DecreaseProgressBar_PlayerHealth();
	}
}

void AInGameManager::UpdatePlayerGunAmmoUI()
{
	if (InGameUI != nullptr)
	{
		sting_LoadedAmmo = FString::FromInt(PlayerCharacter->GetLoadedAmmo());
		sting_MagazineCapacity = FString::FromInt(PlayerCharacter->GetMagazineCapacity());
		sting_LeftAmmo = FString::FromInt(PlayerCharacter->GetLeftAmmo());

		text_LoadedAmmo->SetText(FText::FromString(sting_LoadedAmmo));
		text_MagazineCapacity->SetText(FText::FromString(sting_MagazineCapacity));
		text_LeftAmmo->SetText(FText::FromString(sting_LeftAmmo));
	}
}

void AInGameManager::ShowUI_AimMode()
{
	Panel_AimMode->SetVisibility(ESlateVisibility::Visible);
	Panel_HarvestMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_InventoryMode->SetVisibility(ESlateVisibility::Hidden);
}

void AInGameManager::HideInGameUI()
{
	if (InGameUI != nullptr)
	{
		InGameUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AInGameManager::BindItemBtnWithClickedAction(class UUserWidget* ButtonWidget)
{
	UButton* CurrentItemButton = Cast<UButton>(ButtonWidget->GetWidgetFromName(TEXT("Button_Item")));

	CurrentItemButton->OnClicked.AddDynamic(this, &AInGameManager::ShowSelectedItemButtonInfo);
	 
	//CurrentSelectedButton = Cast<UInventoryItemButton>(ButtonWidget);

	//ShowSelectedItemButtonInfo();
}

void AInGameManager::BindResultPanelWidgets()
{
	CanvasPanel_Result = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_Result"));
	
	TextBlock_ResultQuestContent = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_ResultQuestContent"));
	TextBlock_ResultQuestObjective = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_ResultQuestObjective"));
	TextBlock_ResultContent = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_ResultContent"));
	TextBlock_ResultRewardItem = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_ResultRewardItem"));
	TextBlock_ResultRewardCount = Cast<UTextBlock>(InGameUI->GetWidgetFromName("TextBlock_ResultRewardCount"));
	
	Button_ResultConfirm = Cast<UButton>(InGameUI->GetWidgetFromName("Button_ResultConfirm"));
	Button_ResultConfirm->OnClicked.AddDynamic(this, &AInGameManager::GotoInterMission);
}

void AInGameManager::SetResultPanelWidgets()
{
	if (CurrentQuestTableRow != nullptr)
	{
		TextBlock_ResultQuestContent->SetText(FText::FromString(CurrentQuestTableRow->Quest_Content));
		TextBlock_ResultQuestObjective->SetText(FText::FromString(CurrentQuestTableRow->Objective_Name));
		TextBlock_ResultContent->SetText(FText());
		TextBlock_ResultRewardItem->SetText(FText::FromString(CurrentQuestTableRow->Reward_Name));
		TextBlock_ResultRewardCount->SetText(FText::AsNumber(CurrentQuestTableRow->Reward_Count));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("CurrentQuestTableRow is nullptr.."));
	}
}

// by����, �����۹�ȣ�� ���ڷ� �޾� ������ �������� �����ϴ��� üũ
bool AInGameManager::CheckSameItemExist(int32 ItemNum)
{
	// �κ��丮 �ȿ� ����� �������� 0���� ���
	if (SlotNumToStartAdd < 1)
	{
		// ������ �߰��� ���Թ�ȣ 0������ ����
		SlotNumToStartAdd = 0;
		return false;
	}
	// �κ��丮 �ȿ� ����� �������� 1�� �̻��� ���
	else
	{
		// �κ��丮 �� �����۽����� �˻��Ͽ� ������ �����۹�ȣ�� ����Ǿ��ִ��� üũ
		for (int i = 0; i < SlotNumToStartAdd; i++)
		{
			// �κ��丮 �� ������ ���Կ� ����
			int32 TempNum = Cast<UInventoryItemButton>(
				CurrentInventorySlotArray[i]->GetChildAt(0)
			)->GetCurrentItemNumber();
			// ������ ��ȣ ��
			if (TempNum == ItemNum)
			{
				// ������ ������ ���� �� �ش� ���� ��ȣ�� �����ϰ� true�� ��ȯ��.
				SlotNumToStartAdd = i;
				return true;
			}

		}
		// ������ ������������ ���°�� false�� ��ȯ��.
		return false;
	}
}

// by����, �κ��丮�� �����ϴ� �������� ������ �÷���
void AInGameManager::AddExistItemCount()
{
	// ��Ŀ�̵� �������� ���°�� ȣ������ ����
	if (PlayerCharacter->FocusedItem != nullptr)
	{
		// ȹ���� �����ۿ� �����Ͽ� ȹ�濡 �߰��� ������ �ҷ���.
		int32 TempItemAmountToAdd = PlayerCharacter->FocusedItem->GetCurrentItemAmount();
		
		// �κ��丮 �ȿ� �߰��� �����۽����� ������ ��ư�� �ּҸ� �ҷ���.
		UInventoryItemButton* TempCurrentBtn 
			= Cast<UInventoryItemButton>(CurrentInventorySlotArray[SlotNumToStartAdd]->GetChildAt(0));
		
		// ������ ��ư�� �߰��� ������ ������ ������.
		TempCurrentBtn->AddCurrentItemCount(TempItemAmountToAdd);
		
		// ���� �������� ź���� ��� ĳ������ ����ź�� ������ �÷���
		if (GetFocusedItemNumber() == 2)
		{
			PlayerCharacter->AddLeftAmmo(TempItemAmountToAdd);
		}
		
		// �κ��丮 UI�� ź�� ���� ������ ����.
		TextBlock_CurrentItemName->SetText(FText::FromString(TempCurrentBtn->GetCurrentItemName()));
		TextBlock_CurrentItemCount->SetText(FText::FromString(FString::FromInt(TempCurrentBtn->GetCurrentItemCount())));
		TextBlock_CurrentItemInfo->SetText(FText::FromString(TempCurrentBtn->GetCurrentItemInfo()));
	}
}

UInventoryItemButton* AInGameManager::GetInventoryItemButton_Pistol()
{
	return UInventoryItemButton_PistolAmmo;
}


void AInGameManager::AddQuestObjectiveCollectCountUp()
{
	CurrentQuestObjectiveCollectCount++;
	TextBlock_StageQuestCurrentCount->SetText(FText::AsNumber(CurrentQuestObjectiveCollectCount));

}

void AInGameManager::CheckQuestComplete()
{
	if (CurrentQuestObjectiveCollectCount == CurrentQuestObjectiveCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest succeed.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest not succeed.."));
	}
}

void AInGameManager::CalculateQuestCompletePercent()
{
	if (CurrentQuestObjectiveCollectCount == 0)
	{
		CurrentQuestCompletePercent = 0;
	}

	float TempPercent = float(CurrentQuestObjectiveCollectCount) / float(CurrentQuestObjectiveCount);
	CurrentQuestCompletePercent = int32(TempPercent * 100.0f);

	if (CurrentQuestCompletePercent >= 100)
	{
		CurrentQuestCompletePercent = 100;
		PlayerCharacter->IsCompletedQuest = true;
	}

	TextBlock_StageQuestPercent->SetText(FText::AsNumber(CurrentQuestCompletePercent));
}

void AInGameManager::ShowResultPanel()
{
	CanvasPanel_Result->SetVisibility(ESlateVisibility::Visible);
	Panel_AimMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_HarvestMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_InventoryMode->SetVisibility(ESlateVisibility::Hidden);
}

void AInGameManager::ShowPanel_GameOver()
{
	// by����, �ΰ���UI�� �մϴ�.
	ShowInGameUI();
	// by����, ���ӿ��� �г��� �մϴ�.
	CanvasPanel_GameOver->SetVisibility(ESlateVisibility::Visible);
	// by����, ���ӿ��� �̿��� �г��� ���ϴ�.
	CanvasPanel_Result->SetVisibility(ESlateVisibility::Hidden);
	Panel_AimMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_HarvestMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_InventoryMode->SetVisibility(ESlateVisibility::Hidden);
}


void AInGameManager::GotoInterMission()
{
	//UE_LOG(LogTemp, Display, TEXT("Call GotoInterMission.."));
	UGameplayStatics::OpenLevel(this, FName("InterMission"));
}

void AInGameManager::DeleteInventoryItemButton()
{
	UInventoryItemButton_PistolAmmo->RemoveFromParent();
}

void AInGameManager::ShowItemIconImage(UUserWidget* UerWidget_ItemButton)
{
	UInventoryItemButton* CurrentItemButton = Cast<UInventoryItemButton>(UerWidget_ItemButton);

	if (UerWidget_ItemButton->GetWidgetFromName(TEXT("text_ItemNum")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found text_ItemNum succeed.."));
		FString temp = FString::FromInt(CurrentItemButton->GetCurrentItemNumber());
		Cast<UTextBlock>(UerWidget_ItemButton->GetWidgetFromName(TEXT("text_ItemNum")))->SetText(FText::FromString(temp));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found text_ItemNum failed.."));
	}
}

void AInGameManager::LoadSavedData()
{
	InGameSaveDataPtr = nullptr;
	InGameSaveDataPtr = Cast<USaveGameInfo>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

	if (InGameSaveDataPtr != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load saved data succeed.."));
		CurrentQuestNumber = InGameSaveDataPtr->QuestNumber;
		UE_LOG(LogTemp, Warning, TEXT("Loaded saved number:%d"), CurrentQuestNumber);
		UE_LOG(LogTemp, Warning, TEXT("Loaded saved number:%s"), *InGameSaveDataPtr->QuestName);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load saved data failed.."));
	}
}

// by����, ����Ʈ ������ ���̺��� Ư������ ������ �ҷ��ɴϴ�.
void AInGameManager::LoadQuestDataFromTable(int32 QuestNum)
{
	// QuestNum ��ȣ�� �´� ���� ã�� CurrentQuestTableRow �� �ּҸ� �����մϴ�.
	CurrentQuestTableRow = CurrentQuestDataTable->FindRow<FQuestTableRow>(FName(*(FString::FormatAsNumber(QuestNum))), FString(""));
	
	//�ҷ��� CurrentQuestTableRow ���� ��������� ������ �����ͼ� TextBlock�� �Է��մϴ�.
	TextBlock_StageQuestName->SetText(FText::FromString(CurrentQuestTableRow->Quest_Name));
	TextBlock_StageQuestContent->SetText(FText::FromString(CurrentQuestTableRow->Quest_Content));
	TextBlock_StageQuestObjective->SetText(FText::FromString(CurrentQuestTableRow->Objective_Name));
	TextBlock_StageQuestReward->SetText(FText::FromString(CurrentQuestTableRow->Reward_Name));
	CurrentQuestObjectiveName = CurrentQuestTableRow->Objective_Name;
	TextBlock_StageQuestCurrent->SetText(FText::FromString(CurrentQuestObjectiveName));
	CurrentQuestObjectiveCount = CurrentQuestTableRow->Objective_Count;
	TextBlock_StageQuestObjectiveCount->SetText(FText::AsNumber(CurrentQuestObjectiveCount));
	TextBlock_StageQuestCurrentCount->SetText(FText::AsNumber(CurrentQuestObjectiveCollectCount));
	
	// ���� ����Ʈ �Ϸ����� ����ϴ� �Լ��� ȣ���Ͽ� TextBlock�� �Է��մϴ�.
	CalculateQuestCompletePercent();
	TextBlock_StageQuestPercent->SetText(FText::AsNumber(CurrentQuestCompletePercent));
}

// by����, ��ȭ ������ ���̺��� Ư������ ������ �ҷ��ɴϴ�.
void AInGameManager::LoadDialogueDataFromTable(int32 ObjectNum)
{
	//ObjectNum 1, IronFenceGate
	CurrentDialogueTableRowNum = ObjectNum;
	CurrentDialogueTableRow = CurrentDialogueDataTable->FindRow<FDialogueTableRow>(FName(*(FString::FormatAsNumber(CurrentDialogueTableRowNum))), FString(""));

	Text_Interact_Description->SetText(FText::FromString(CurrentDialogueTableRow->Description));


	/*int32 TempSearchRowNum = 1;
	int32 TempTableNum = CurrentDialogueDataTable->GetRowNames().Num();
	UE_LOG(LogTemp, Display, TEXT("CurrentDialogueDataTable RowNum:%d"), TempTableNum);
	
	while (TempSearchRowNum <= TempTableNum)
	{
		CurrentDialogueTableRow = CurrentDialogueDataTable->FindRow<FDialogueTableRow>(FName(*(FString::FormatAsNumber(TempSearchRowNum))), FString(""));

		if (CurrentDialogueTableRow->Object_Number == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("TempSearchRowNum:%d"), TempSearchRowNum);
			TempSearchRowNum++;
		}
		else
		{
			Text_Interact_Description->SetText(FText::FromString(CurrentDialogueTableRow->Description));
			return;
		}
	}*/

}

void AInGameManager::SetCurrentGameModeBase()
{
	CurrentGameModeBase = Cast<AProjectMSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (CurrentGameModeBase != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load CurrentGameModeBase succeed.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load CurrentGameModeBase failed.."));
	}
}

// ����Ʈ ������ ���̺� �ε带 üũ�մϴ�.
void AInGameManager::SetCurrentQuestDataTable()
{
	CurrentQuestDataTable = CurrentGameModeBase->QuestDataTable;

	if (CurrentQuestDataTable != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load Current QuestDataTable succeed.."));

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load Current QuestDataTable failed.."));
	}
}

void AInGameManager::SetCurrentDialogueDataTable()
{
	CurrentDialogueDataTable = CurrentGameModeBase->DialogueDataTable;

	if (CurrentDialogueDataTable != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load Current DialogueDataTable succeed.."));

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load Current DialogueDataTable failed.."));
	}
}

void AInGameManager::MakeItemBtnStrings(int32 ItemNumber, FString TempItemName, FString TempItemInfo, UInventoryItemButton* InventoryItemButton, int32 TempItemAmountToAdd)
{
	if (ItemNumber == 0)
	{
		TempItemName = FString("Amino");
		TempItemInfo = FString("This is good for health");
	}
	else if (ItemNumber == 1)
	{
		TempItemName = FString("ToolBox");
		TempItemInfo = FString("This is ToolBox");
	}
	else if (ItemNumber == 2)
	{
		TempItemName = FString("PistolAmmo");
		TempItemInfo = FString("This is PistolAmmo");

		UInventoryItemButton_PistolAmmo = InventoryItemButton;
		PlayerCharacter->AddLeftAmmo(TempItemAmountToAdd);
	}
	else if (ItemNumber == 3)
	{
		TempItemName = FString("Key");
		TempItemInfo = FString("This can open the gate");
	}
	else
	{
		TempItemName = FString("Unknown");
		TempItemInfo = FString("This is unknown item");
	}

	InventoryItemButton->SetCurrentItemName(TempItemName);
	InventoryItemButton->SetCurrentItemInfo(TempItemInfo);

	TextBlock_CurrentItemName->SetText(FText::FromString(InventoryItemButton->GetCurrentItemName()));
	TextBlock_CurrentItemCount->SetText(FText::FromString(FString::FromInt(InventoryItemButton->GetCurrentItemCount())));
	TextBlock_CurrentItemInfo->SetText(FText::FromString(InventoryItemButton->GetCurrentItemInfo()));
}

// by����, �κ��丮���� ������ ���� ȣ��Ǵ� �Լ��Դϴ�.
void AInGameManager::UseSelectedItem()
{
	// ��ư �ν��Ͻ� �ּҰ� ����ִ��� üũ�մϴ�.
	if (CurrentSelectedButton != nullptr)
	{
		// ���� ���õ� ��ư�� �����۹�ȣ�� 0������ üũ�մϴ�.
		if (CurrentSelectedButton->GetCurrentItemNumber() == 0)
		{
			// �����۹�ȣ�� 0�� ��� ĳ������ ü���� 25��ŭ ȸ�������ݴϴ�.
			PlayerCharacter->AddCurrentHP(25.0f);
			// ���� ĳ������ ü�� ��ġ Ȯ�ο� �α�ȣ�� �Լ��Դϴ�.
			UE_LOG(LogTemp, Display, TEXT("CurrentHP:%f"), PlayerCharacter->GetCurrentHP());
		}
	}
}

void AInGameManager::CloseInventoryWindow()
{
	PlayerCharacter->InventorySwitch();
}

void AInGameManager::ShowSelectedItemButtonInfo()
{
	if (CurrentSelectedButton != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Find selected button's parent succeed.."));
		
		TextBlock_CurrentItemName->SetText(FText::FromString(CurrentSelectedButton->GetCurrentItemName()));
		TextBlock_CurrentItemCount->SetText(FText::FromString(FString::FromInt(CurrentSelectedButton->GetCurrentItemCount())));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find selected button's parent failed.."));
	}
}

void AInGameManager::DecreaseProgressBar_PlayerHealth()
{
	//UProgressBar_PlayerHealth->Percent -= 0.1f;
	UProgressBar_PlayerHealth->SetPercent(0.5f);
}

void AInGameManager::ShowUI_HarvestMode()
{
	Panel_AimMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_HarvestMode->SetVisibility(ESlateVisibility::Visible);
	Panel_InventoryMode->SetVisibility(ESlateVisibility::Hidden);
}

void AInGameManager::HideUI_HarvestMode()
{
	Panel_HarvestMode->SetVisibility(ESlateVisibility::Hidden);
}

void AInGameManager::ShowUI_InventoryMode()
{
	Panel_AimMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_HarvestMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_InventoryMode->SetVisibility(ESlateVisibility::Visible);
}

void AInGameManager::ShowUI_ItemGet()
{
	// by����, ������ ȹ��UI�� ���̵��� Visible�� �����մϴ�.
	Panel_ItemGet->SetVisibility(ESlateVisibility::Visible);
	// by����, �ֱ� ȹ���� �������� ������ȣ�� �����ͼ� TextBlock�� ����մϴ�.
	TextBlock_GetItemName->SetText(FText::AsNumber(GetFocusedItemNumber()));
	// by����, ���� InGameUITimerHandle�� Ȱ��ȭ �Ǿ����� üũ�ϰ� ��Ȱ�� ���¶�� ����մϴ�.
	if (GetWorld()->GetTimerManager().IsTimerActive(IngameUITimerHandle) == false)
	{
		//by����, 3���� HideUI_ItemGet �Լ��� �ѹ� ȣ���ϵ��� �մϴ�.
		GetWorld()->GetTimerManager().SetTimer(IngameUITimerHandle, this, &AInGameManager::HideUI_ItemGet, 3.0f, false);
	}
}

void AInGameManager::HideUI_ItemGet()
{
	// by����, ������ ȹ��ǥ�� UI�� �ڽ����� ������ �ִ� �г��� Hidden���� ��Ȱ��ȭ �����ݴϴ�.
	Panel_ItemGet->SetVisibility(ESlateVisibility::Hidden);
}

void AInGameManager::SetImage_GetItemInfo()
{
	// by����, ��Ŀ�� �� �������� �̹����� GetItem�г� �̹��� ���Կ� �ֽ��ϴ�.
	Image_ItemInfo->SetBrushFromTexture(
		ItemInfoImage[FocusedItemNumber]
	);
}

void AInGameManager::ShowUI_GetItemInfo()
{
	CanvasPanel_ItemInfo->SetVisibility(ESlateVisibility::Visible);
}

void AInGameManager::HideUI_GetItemInfo()
{
	CanvasPanel_ItemInfo->SetVisibility(ESlateVisibility::Hidden);
	PlayerCharacter->SetGamePlayModeControl();
}

void AInGameManager::ShowUI_Interaction()
{
	CurrentDialogueTableRowNum = 1;
	
	LoadDialogueDataFromTable(CurrentDialogueTableRowNum);

	CanvasPanel_Interaction->SetVisibility(ESlateVisibility::Visible);
	CanvasPanel_Result->SetVisibility(ESlateVisibility::Hidden);
	Panel_ItemGet->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_ItemInfo->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_GameOver->SetVisibility(ESlateVisibility::Hidden);

	Panel_AimMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_HarvestMode->SetVisibility(ESlateVisibility::Hidden);
	Panel_InventoryMode->SetVisibility(ESlateVisibility::Hidden);

	Button_Interaction_yes->SetVisibility(ESlateVisibility::Visible);
	
}

// by����, ����ƮŰ ȹ�濩�ο� ���� ���� ��ȭ���� �ٲ��ִ� �Լ��Դϴ�.
void AInGameManager::ShowNextUI_Interaction()
{
	// �÷��̾ ����Ʈ ���踦 ������ ���� ���� ���
	if (PlayerCharacter->HasGateKey == false)
	{
		//��ȭ�� ������ ���̺��� ���� ���� �о�ɴϴ�.
		LoadDialogueDataFromTable(CurrentDialogueTableRowNum + 1);
	}
	// �÷��̾ ����Ʈ ���踦 ������ �ִ� ���
	else
	{
		//��ȭ�� ������ ���̺��� ���� ���� ���� �о�ɴϴ�.
		LoadDialogueDataFromTable(CurrentDialogueTableRowNum + 2);
	}
	// ���� �÷��̾�� Ȱ��ȭ �Ǿ��ִ� ����Ʈ�� �۵���ŵ�ϴ�.
	PlayerCharacter->ActiveCurrentGate();
	// ����Ʈ ��ȣ�ۿ� UI�� �ʱ�ȭ �մϴ�.
	Button_Interaction_yes->SetVisibility(ESlateVisibility::Hidden);
}

void AInGameManager::HideUI_Interaction()
{
	CanvasPanel_Interaction->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_Result->SetVisibility(ESlateVisibility::Hidden);
	Panel_ItemGet->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_ItemInfo->SetVisibility(ESlateVisibility::Hidden);
	CanvasPanel_GameOver->SetVisibility(ESlateVisibility::Hidden);

	PlayerCharacter->DismissDialogu();
}

void AInGameManager::IncreaseProgressBar_Harvest(float PercentToAdd)
{
	UProgressBar_Harvest->SetPercent(UProgressBar_Harvest->GetPercent() + PercentToAdd);
}

void AInGameManager::ResetProgressBar_Harvest()
{
	UProgressBar_Harvest->SetPercent(0);
}

// ������ �����۹�ư�� �κ��丮 ���� �ش� �������� �̵���Ű�� �Լ�
void AInGameManager::AddItemtoInventory()
{
	// ���� �κ��丮�� �� á�� ��� �Լ��� �����մϴ�.
	if (SlotNumToStartAdd == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is already full.."));
		return;
	}
	// ȹ���� �������� ��ȣ�� ���ڷ� �޾Ƽ� �ش��ϴ� �������� ���Թ�ȣ�� �˻��մϴ�.
	CheckSameItemExist(GetFocusedItemNumber());
	// ���Թ�ȣ�� ���ڿ��� �����Ͽ� �ش��ν��Ͻ� �̸��� �����մϴ�.
	FString TempName = TEXT("Border_Slot_");
	TempName.AppendInt(SlotNumToStartAdd);
	FName TestText = FName(TempName);
	// ���� �̸��� �˻��Ͽ�, �ش罽�Կ� �����۹�ư �ν��Ͻ��� �ڽ����� �߰��մϴ�.
	CurrentInventorySlotArray[SlotNumToStartAdd] = Cast<UBorder>(InGameUI->GetWidgetFromName(TestText));
	CurrentInventorySlotArray[SlotNumToStartAdd]->AddChild(ItemButton);
	ShowItemIconImage(ItemButton);
}

// �κ��丮 �ȿ� ������ ��ư�� �����մϴ�.
void AInGameManager::CreateItemButton(int32 ItemNumber)
{
	if (GameItemButtonClass != nullptr)
	{
		// �����۹�ư ���� �ν��Ͻ��� �����ϰ� �ּҰ��� �����մϴ�.
		ItemButton = CreateWidget(GetWorld(), GameItemButtonClass);
		UInventoryItemButton* InventoryItemButton 
			= Cast<UInventoryItemButton>(ItemButton);	
		
		// ȹ���� �����ۿ��� ���� ���� ������ ������ �����մϴ�.
		int32 TempItemAmountToAdd = 0;
		if (PlayerCharacter->FocusedItem != nullptr)
		{
			TempItemAmountToAdd = PlayerCharacter->FocusedItem->GetCurrentItemAmount();
		}
		// ������ ��ư �ν��Ͻ� �ȿ� �����۹�ȣ, ���ؾ��� ������ �����մϴ�.
		InventoryItemButton->SetCurrentItemNumber(ItemNumber);
		InventoryItemButton->AddCurrentItemCount(TempItemAmountToAdd);
		InventoryItemButton->SetCurrentInGameManager(this);
		// �����۹�ư�� �������� �̸��� ������ �����մϴ�.
		FString TempItemName;
		FString TempItemInfo;
		MakeItemBtnStrings(ItemNumber, TempItemName, TempItemInfo, InventoryItemButton, TempItemAmountToAdd);
		// �����۹�ư ���� üũ�� �α��Դϴ�.
		UE_LOG(LogTemp, Warning, TEXT("Current Item number:%d, Count: %d"), 
			InventoryItemButton->GetCurrentItemNumber(), InventoryItemButton->GetCurrentItemCount());
	}
	else
	{
		// �����۹�ư �������� üũ�� �α��Դϴ�.
		UE_LOG(LogTemp, Warning, TEXT("Create ItemButton failed.."));
	}
}


void AInGameManager::DecreasePistolAmmoButtonCountInfo()
{
	UInventoryItemButton_PistolAmmo->DecreaseCurrentItemCount(1);
}

void AInGameManager::CreateInGameUI()
{
	// by����, TsubclassŸ�� Ŭ�������� InGameUIClass�� nullptr�� �ƴ� ��� ������ �����մϴ�.
	if (InGameUIClass != nullptr)
	{
		// by����, CreateWidget���� ������ �ν��Ͻ� �ּҸ� InGameUI�� �����մϴ�.
		InGameUI = CreateWidget(GetWorld(), InGameUIClass);

		// by����, ���� ���� Ȯ�ο� �ڵ��Դϴ�.
		UE_LOG(LogTemp, Warning, TEXT("Create InGameUI succeed.."));

		// by����, ������ UI�� ����Ʈ�� �߰��ǵ��� �մϴ�.
		InGameUI->AddToViewport();

		// by����, ������ UI�� �ٷ� �������� �ʵ��� Hidden���·� �����մϴ�.
		InGameUI->SetVisibility(ESlateVisibility::Hidden);

		// by����, UI ������ TextBlock�� Button�� ������ �� �ֵ���, ��ũ��Ʈ ������ ����UI�ּҸ� �Ҵ��մϴ�.
		BindWidget();

		// by����, �÷��̾ �������� ���� ź�� ���� �ʱ�ȭ ������ UI���� ��Ÿ������ �մϴ�.
		InitPlayerGunAmmo();
	}
	else
	{
		// by����, UI �ν��Ͻ� ���� ���н� �˷��ִ� Ȯ�ο� �ڵ��Դϴ�.
		UE_LOG(LogTemp, Warning, TEXT("Create InGameUI failed.."));
	}
}

void AInGameManager::SetFocusedItemNumber(int32 ItemNumber)
{
	FocusedItemNumber = ItemNumber;
}

int32 AInGameManager::GetFocusedItemNumber()
{
	return FocusedItemNumber;
}



