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
	// by정훈, 게임오버 패널의 주소를 저장합니다.
	CanvasPanel_GameOver = Cast<UCanvasPanel>(InGameUI->GetWidgetFromName("CanvasPanel_GameOver"));
	// by정훈, 게임오버 패널의 타이틀화면 버튼 주소를 저장합니다.
	Button_GotoTitle = Cast<UButton>(InGameUI->GetWidgetFromName(TEXT("Button_GotoTitle")));
	// by정훈, 게임오버 패널의 타이틀화면 버튼의 클릭이벤트에 함수를 바인드합니다.
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

// by정훈, 아이템번호를 인자로 받아 동일한 아이템이 존재하는지 체크
bool AInGameManager::CheckSameItemExist(int32 ItemNum)
{
	// 인벤토리 안에 저장된 아이템이 0개인 경우
	if (SlotNumToStartAdd < 1)
	{
		// 아이템 추가할 슬롯번호 0번으로 저장
		SlotNumToStartAdd = 0;
		return false;
	}
	// 인벤토리 안에 저장된 아이템이 1개 이상인 경우
	else
	{
		// 인벤토리 내 아이템슬롯을 검색하여 동일한 아이템번호가 저장되어있는지 체크
		for (int i = 0; i < SlotNumToStartAdd; i++)
		{
			// 인벤토리 내 아이템 슬롯에 접근
			int32 TempNum = Cast<UInventoryItemButton>(
				CurrentInventorySlotArray[i]->GetChildAt(0)
			)->GetCurrentItemNumber();
			// 아이템 번호 비교
			if (TempNum == ItemNum)
			{
				// 동일한 아이템 존재 시 해당 슬롯 번호를 저장하고 true로 반환함.
				SlotNumToStartAdd = i;
				return true;
			}

		}
		// 동일한 아이템종류가 없는경우 false로 반환함.
		return false;
	}
}

// by정훈, 인벤토리에 존재하는 아이템의 수량을 늘려줌
void AInGameManager::AddExistItemCount()
{
	// 포커싱된 아이템이 없는경우 호출하지 않음
	if (PlayerCharacter->FocusedItem != nullptr)
	{
		// 획득한 아이템에 접근하여 획득에 추가할 수량을 불러옴.
		int32 TempItemAmountToAdd = PlayerCharacter->FocusedItem->GetCurrentItemAmount();
		
		// 인벤토리 안에 추가할 아이템슬롯의 아이템 버튼의 주소를 불러옴.
		UInventoryItemButton* TempCurrentBtn 
			= Cast<UInventoryItemButton>(CurrentInventorySlotArray[SlotNumToStartAdd]->GetChildAt(0));
		
		// 아이템 버튼에 추가할 아이템 수량을 더해줌.
		TempCurrentBtn->AddCurrentItemCount(TempItemAmountToAdd);
		
		// 현재 아이템이 탄약일 경우 캐릭터의 소지탄약 수량을 늘려줌
		if (GetFocusedItemNumber() == 2)
		{
			PlayerCharacter->AddLeftAmmo(TempItemAmountToAdd);
		}
		
		// 인벤토리 UI에 탄약 수량 정보를 갱신.
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
	// by정훈, 인게임UI를 켭니다.
	ShowInGameUI();
	// by정훈, 게임오버 패널을 켭니다.
	CanvasPanel_GameOver->SetVisibility(ESlateVisibility::Visible);
	// by정훈, 게임오버 이외의 패널을 끕니다.
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

// by정훈, 퀘스트 데이터 테이블에서 특정행의 정보를 불러옵니다.
void AInGameManager::LoadQuestDataFromTable(int32 QuestNum)
{
	// QuestNum 번호에 맞는 행을 찾아 CurrentQuestTableRow 에 주소를 저장합니다.
	CurrentQuestTableRow = CurrentQuestDataTable->FindRow<FQuestTableRow>(FName(*(FString::FormatAsNumber(QuestNum))), FString(""));
	
	//불러온 CurrentQuestTableRow 안의 멤버변수를 정보를 가져와서 TextBlock에 입력합니다.
	TextBlock_StageQuestName->SetText(FText::FromString(CurrentQuestTableRow->Quest_Name));
	TextBlock_StageQuestContent->SetText(FText::FromString(CurrentQuestTableRow->Quest_Content));
	TextBlock_StageQuestObjective->SetText(FText::FromString(CurrentQuestTableRow->Objective_Name));
	TextBlock_StageQuestReward->SetText(FText::FromString(CurrentQuestTableRow->Reward_Name));
	CurrentQuestObjectiveName = CurrentQuestTableRow->Objective_Name;
	TextBlock_StageQuestCurrent->SetText(FText::FromString(CurrentQuestObjectiveName));
	CurrentQuestObjectiveCount = CurrentQuestTableRow->Objective_Count;
	TextBlock_StageQuestObjectiveCount->SetText(FText::AsNumber(CurrentQuestObjectiveCount));
	TextBlock_StageQuestCurrentCount->SetText(FText::AsNumber(CurrentQuestObjectiveCollectCount));
	
	// 현재 퀘스트 완료율을 계산하는 함수를 호출하여 TextBlock에 입력합니다.
	CalculateQuestCompletePercent();
	TextBlock_StageQuestPercent->SetText(FText::AsNumber(CurrentQuestCompletePercent));
}

// by정훈, 대화 데이터 테이블에서 특정행의 정보를 불러옵니다.
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

// 퀘스트 데이터 테이블 로드를 체크합니다.
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

// by정훈, 인벤토리에서 아이템 사용시 호출되는 함수입니다.
void AInGameManager::UseSelectedItem()
{
	// 버튼 인스턴스 주소가 비어있는지 체크합니다.
	if (CurrentSelectedButton != nullptr)
	{
		// 현재 선택된 버튼의 아이템번호가 0번인지 체크합니다.
		if (CurrentSelectedButton->GetCurrentItemNumber() == 0)
		{
			// 아이템번호가 0일 경우 캐릭터의 체력을 25만큼 회복시켜줍니다.
			PlayerCharacter->AddCurrentHP(25.0f);
			// 현재 캐릭터의 체력 수치 확인용 로그호출 함수입니다.
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
	// by정훈, 아이템 획득UI가 보이도록 Visible로 설정합니다.
	Panel_ItemGet->SetVisibility(ESlateVisibility::Visible);
	// by정훈, 최근 획득한 아이템의 고유번호를 가져와서 TextBlock에 출력합니다.
	TextBlock_GetItemName->SetText(FText::AsNumber(GetFocusedItemNumber()));
	// by정훈, 현재 InGameUITimerHandle이 활성화 되었는지 체크하고 비활성 상태라면 명령합니다.
	if (GetWorld()->GetTimerManager().IsTimerActive(IngameUITimerHandle) == false)
	{
		//by정훈, 3초후 HideUI_ItemGet 함수를 한번 호출하도록 합니다.
		GetWorld()->GetTimerManager().SetTimer(IngameUITimerHandle, this, &AInGameManager::HideUI_ItemGet, 3.0f, false);
	}
}

void AInGameManager::HideUI_ItemGet()
{
	// by정훈, 아이템 획득표시 UI를 자식으로 가지고 있는 패널을 Hidden으로 비활성화 시켜줍니다.
	Panel_ItemGet->SetVisibility(ESlateVisibility::Hidden);
}

void AInGameManager::SetImage_GetItemInfo()
{
	// by정훈, 포커스 된 아이템의 이미지를 GetItem패널 이미지 슬롯에 넣습니다.
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

// by정훈, 게이트키 획득여부에 따라 다음 대화문을 바꿔주는 함수입니다.
void AInGameManager::ShowNextUI_Interaction()
{
	// 플레이어가 게이트 열쇠를 가지고 있지 않은 경우
	if (PlayerCharacter->HasGateKey == false)
	{
		//대화문 데이터 테이블의 다음 행을 읽어옵니다.
		LoadDialogueDataFromTable(CurrentDialogueTableRowNum + 1);
	}
	// 플레이어가 게이트 열쇠를 가지고 있는 경우
	else
	{
		//대화문 데이터 테이블의 다음 다음 행을 읽어옵니다.
		LoadDialogueDataFromTable(CurrentDialogueTableRowNum + 2);
	}
	// 현재 플레이어에게 활성화 되어있는 게이트를 작동시킵니다.
	PlayerCharacter->ActiveCurrentGate();
	// 게이트 상호작용 UI를 초기화 합니다.
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

// 생성한 아이템버튼을 인벤토리 안의 해당 슬롯으로 이동시키는 함수
void AInGameManager::AddItemtoInventory()
{
	// 현재 인벤토리가 다 찼을 경우 함수를 종료합니다.
	if (SlotNumToStartAdd == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is already full.."));
		return;
	}
	// 획득한 아이템의 번호를 인자로 받아서 해당하는 아이템의 슬롯번호를 검색합니다.
	CheckSameItemExist(GetFocusedItemNumber());
	// 슬롯번호와 문자열을 조합하여 해당인스턴스 이름을 저장합니다.
	FString TempName = TEXT("Border_Slot_");
	TempName.AppendInt(SlotNumToStartAdd);
	FName TestText = FName(TempName);
	// 슬롯 이름을 검색하여, 해당슬롯에 아이템버튼 인스턴스를 자식으로 추가합니다.
	CurrentInventorySlotArray[SlotNumToStartAdd] = Cast<UBorder>(InGameUI->GetWidgetFromName(TestText));
	CurrentInventorySlotArray[SlotNumToStartAdd]->AddChild(ItemButton);
	ShowItemIconImage(ItemButton);
}

// 인벤토리 안에 아이템 버튼을 생성합니다.
void AInGameManager::CreateItemButton(int32 ItemNumber)
{
	if (GameItemButtonClass != nullptr)
	{
		// 아이템버튼 위젯 인스턴스를 생성하고 주소값을 저장합니다.
		ItemButton = CreateWidget(GetWorld(), GameItemButtonClass);
		UInventoryItemButton* InventoryItemButton 
			= Cast<UInventoryItemButton>(ItemButton);	
		
		// 획득한 아이템에서 수량 값을 가져와 변수에 저장합니다.
		int32 TempItemAmountToAdd = 0;
		if (PlayerCharacter->FocusedItem != nullptr)
		{
			TempItemAmountToAdd = PlayerCharacter->FocusedItem->GetCurrentItemAmount();
		}
		// 아이템 버튼 인스턴스 안에 아이템번호, 더해야할 수량을 저장합니다.
		InventoryItemButton->SetCurrentItemNumber(ItemNumber);
		InventoryItemButton->AddCurrentItemCount(TempItemAmountToAdd);
		InventoryItemButton->SetCurrentInGameManager(this);
		// 아이템버튼에 아이템의 이름과 정보를 저장합니다.
		FString TempItemName;
		FString TempItemInfo;
		MakeItemBtnStrings(ItemNumber, TempItemName, TempItemInfo, InventoryItemButton, TempItemAmountToAdd);
		// 아이템버튼 생성 체크용 로그입니다.
		UE_LOG(LogTemp, Warning, TEXT("Current Item number:%d, Count: %d"), 
			InventoryItemButton->GetCurrentItemNumber(), InventoryItemButton->GetCurrentItemCount());
	}
	else
	{
		// 아이템버튼 생성실패 체크용 로그입니다.
		UE_LOG(LogTemp, Warning, TEXT("Create ItemButton failed.."));
	}
}


void AInGameManager::DecreasePistolAmmoButtonCountInfo()
{
	UInventoryItemButton_PistolAmmo->DecreaseCurrentItemCount(1);
}

void AInGameManager::CreateInGameUI()
{
	// by정훈, Tsubclass타입 클래스변수 InGameUIClass가 nullptr이 아닌 경우 위젯을 생성합니다.
	if (InGameUIClass != nullptr)
	{
		// by정훈, CreateWidget으로 생성한 인스턴스 주소를 InGameUI에 저장합니다.
		InGameUI = CreateWidget(GetWorld(), InGameUIClass);

		// by정훈, 위젯 생성 확인용 코드입니다.
		UE_LOG(LogTemp, Warning, TEXT("Create InGameUI succeed.."));

		// by정훈, 생성한 UI가 뷰포트에 추가되도록 합니다.
		InGameUI->AddToViewport();

		// by정훈, 생성한 UI가 바로 보여지지 않도록 Hidden상태로 설정합니다.
		InGameUI->SetVisibility(ESlateVisibility::Hidden);

		// by정훈, UI 내부의 TextBlock과 Button에 접근할 수 있도록, 스크립트 변수에 내부UI주소를 할당합니다.
		BindWidget();

		// by정훈, 플레이어가 장착중인 총의 탄약 수량 초기화 정보를 UI에서 나타내도록 합니다.
		InitPlayerGunAmmo();
	}
	else
	{
		// by정훈, UI 인스턴스 생성 실패시 알려주는 확인용 코드입니다.
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



