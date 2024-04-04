// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomDataTables.h"
#include "InGameManager.generated.h"

UCLASS()
class PROJECTMS_API AInGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> InGameUIClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> GameItemButtonClass;

	// by정훈, 획득한 아이템 설명팝업 인스턴스 주소를 저장할 포인터.
	class UCanvasPanel* CanvasPanel_ItemInfo;

	class UCanvasPanel* CanvasPanel_GameOver;

	class UCanvasPanel* CanvasPanel_Interaction;

	// by정훈, 아이템 설명팝업 내 확인버튼 인스턴스 주소를 저장할 포인터.
	class UButton* Button_ItemInfo_Confirm;

	class UButton* Button_GotoTitle;

	// by정훈, UI 내 이미지 슬롯 주소를 저장할 포인터.
	class UImage* Image_ItemInfo;

	// by정훈, 아이템 텍스쳐 변수.
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	class UTexture2D* ItemInfoImage[6];
	
	UUserWidget* InGameUI;
	UUserWidget* ItemButton;

	class AProjectMSCharacter* PlayerCharacter;

	FString sting_LoadedAmmo;
	FString sting_LeftAmmo;
	FString sting_MagazineCapacity;

	void BindWidget();
	void BindResultPanelWidgets();
	void SetResultPanelWidgets();
	void BindItemBtnWithClickedAction(class UUserWidget* ButtonWidget);

	TArray<class UBorder*> CurrentInventorySlotArray;

	int32 FocusedItemNumber;
	int32 SlotNumToStartAdd;

	void InitInventorySlotArray();
	void ShowItemIconImage(UUserWidget* UerWidget_ItemButton);

	class UTextBlock* TextBlock_CurrentItemName;
	class UTextBlock* TextBlock_CurrentItemCount;
	class UTextBlock* TextBlock_CurrentItemInfo;
	class UInventoryItemButton* UInventoryItemButton_PistolAmmo;

	int32 CurrentQuestNumber;
	FString SaveSlotName = TEXT("Save01");

	class AProjectMSGameMode* CurrentGameModeBase;
	void SetCurrentGameModeBase();
	
	// 불러온 퀘스트 데이터 테이블을 저장한 변수입니다.
	class UDataTable* CurrentQuestDataTable;
	// 불러올 퀘스트 테이블의 행 주소를 저장할 변수입니다.
	struct FQuestTableRow* CurrentQuestTableRow;
	// 퀘스트 데이터 테이블을 저장하는 함수입니다.
	void SetCurrentQuestDataTable();
	// 퀘스트 데이터 테이블에서 특정 행의 정보를 불러오는 함수입니다.
	void LoadQuestDataFromTable(int32 QuestNum);

	// 대화 데이터 테이블을 저장할 변수입니다.
	class UDataTable* CurrentDialogueDataTable;
	// 불러올 대화 데이터 테이블의 행 주소를 저장할 변수입니다.
	struct FDialogueTableRow* CurrentDialogueTableRow;
	// 현재 읽고있는 대화 데이터 테이블의 번호를 저장할 변수.
	int32 CurrentDialogueTableRowNum;
	// 대화 데이터 테이블을 저장하는 함수입니다.
	void SetCurrentDialogueDataTable();
	// 대화 데이터 테이블에서 특정 행의 정보를 불러오는 함수입니다.
	void LoadDialogueDataFromTable(int32 ObjectNum);

	void LoadSavedData();


	class UTextBlock* TextBlock_StageQuestName;
	class UTextBlock* TextBlock_StageQuestContent;
	class UTextBlock* TextBlock_StageQuestObjective;
	class UTextBlock* TextBlock_StageQuestReward;
	class UTextBlock* TextBlock_StageQuestCurrent;
	class UTextBlock* TextBlock_StageQuestPercent;
	class UTextBlock* TextBlock_StageQuestObjectiveCount;
	class UTextBlock* TextBlock_StageQuestCurrentCount;

	FString CurrentQuestObjectiveName;
	int32 CurrentQuestObjectiveCount;
	int32 CurrentQuestObjectiveCollectCount;
	int32 CurrentQuestCompletePercent;

	bool IsCompleteQuest;

	class UCanvasPanel* CanvasPanel_Result;
	class UTextBlock* TextBlock_ResultQuestContent;
	class UTextBlock* TextBlock_ResultQuestObjective;
	class UTextBlock* TextBlock_ResultContent;
	class UTextBlock* TextBlock_ResultRewardItem;
	class UTextBlock* TextBlock_ResultRewardCount;
	class UButton* Button_ResultConfirm;

	class UButton* Button_UseItem;
	
	class UButton* Button_InventoryClose;

	void MakeItemBtnStrings(int32 ItemNumber, FString TempItemName, FString TempItemInfo, UInventoryItemButton* InventoryItemButton, int32 TempItemAmountToAdd);

	void BindWidget_Interaction();
	void BindWidget_StageQuest();
	void BindWidget_GameOver();
	void BindWidget_Inventory();
	void BindWidget_ItemPickupWindow();
	void BindWidget_ItemHarvesting();
	void BindWidget_AimHUD();
	void BindWidget_ItemGetPopup();

public:
	UFUNCTION(BlueprintCallable)
	void ShowSelectedItemButtonInfo();

	UFUNCTION(BlueprintCallable)
	void GotoInterMission();

	class UInventoryItemButton* CurrentSelectedButton;
	
	class USaveGameInfo* InGameSaveDataPtr;
	
	void DecreasePistolAmmoButtonCountInfo();
	
	void InitPlayerGunAmmo();
	void UpdatePlayerGunAmmoUI();
	
	void ShowInGameUI();
	void HideInGameUI();

	class UTextBlock* text_LoadedAmmo;
	class UTextBlock* text_MagazineCapacity;
	class UTextBlock* text_LeftAmmo;

	class UProgressBar* UProgressBar_PlayerHealth;

	void DecreaseProgressBar_PlayerHealth();

	class UGridPanel* Panel_InventorySlotPanel;
	class UCanvasPanel* Panel_AimMode;
	class UCanvasPanel* Panel_HarvestMode;
	class UCanvasPanel* Panel_InventoryMode;
	class UCanvasPanel* Panel_ItemGet;

	void ShowUI_AimMode();
	void ShowUI_HarvestMode();
	void HideUI_HarvestMode();
	void ShowUI_InventoryMode();
	
	FTimerHandle IngameUITimerHandle;
	class UTextBlock* TextBlock_GetItemName;

	void ShowUI_ItemGet();
	void HideUI_ItemGet();

	class UTextBlock* Text_Interact_Description;
	void ShowUI_Interaction();

	UFUNCTION(BlueprintCallable)
	void HideUI_Interaction();

	UFUNCTION(BlueprintCallable)
	void ShowNextUI_Interaction();

	class UButton* Button_Interaction_yes;
	class UButton* Button_Interaction_no;

	void ShowUI_GetItemInfo();

	UFUNCTION(BlueprintCallable)
	void HideUI_GetItemInfo();

	UFUNCTION(BlueprintCallable)
	void UseSelectedItem();

	UFUNCTION(BlueprintCallable)
	void CloseInventoryWindow();

	class UProgressBar* UProgressBar_Harvest;

	void IncreaseProgressBar_Harvest(float PercentToAdd);
	void ResetProgressBar_Harvest();

	void AddItemtoInventory();
	
	void CreateItemButton(int32 ItemNumber);
	void CreateInGameUI();

	void SetFocusedItemNumber(int32 ItemNumber);
	int32 GetFocusedItemNumber();

	bool CheckSameItemExist(int32 ItemNum);

	void AddExistItemCount();
	int32 CheckEmptyInventorySlotNum();

	void DeleteInventoryItemButton();
	class UInventoryItemButton* GetInventoryItemButton_Pistol();

	void AddQuestObjectiveCollectCountUp();
	void CheckQuestComplete();

	void CalculateQuestCompletePercent();
	
	void ShowResultPanel();

	void SetImage_GetItemInfo();

	void ShowPanel_GameOver();
};
