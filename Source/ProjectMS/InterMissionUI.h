// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InterMissionUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMS_API UInterMissionUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

private:
	class UButton* btn_GameStart;
	class UButton* btn_Quest;
	class UButton* btn_Back;
	class UButton* btn_QuestConfirm;

	class UCanvasPanel* CanvasPanel_InterMissionMain;
	class UCanvasPanel* CanvasPanel_IntermissionQuest;

	FString SaveSlotName = TEXT("Save01");

	class ADataTableTestGameModeBase* CurrentGameModeBase;
	class UDataTable* CurrentQuestDataTable;

	TArray<class UButton*> btn_QuestBoard;
	void InitQuestBoardButtons();

	int32 SelectedQuestButtonNum;

	class UTextBlock* TextBlock_QuestName;
	class UTextBlock* TextBlock_ClientName;
	class UTextBlock* TextBlock_QuestStory;
	class UTextBlock* TextBlock_ObjectiveName;
	class UTextBlock* TextBlock_ObjectiveCount;
	class UTextBlock* TextBlock_RewardName;
	class UTextBlock* TextBlock_RewardCount;

public:
	void BindWidget_InterMission();

	UFUNCTION(BlueprintCallable) 
	void GotoIngame();

	UFUNCTION(BlueprintCallable)
	void ShowQuestBoard();

	UFUNCTION(BlueprintCallable)
	void ShowMainScreen();

	UFUNCTION(BlueprintCallable)
	void SaveCurrentQuest();

	UFUNCTION(BlueprintCallable)
	void LoadCurrentQuest();

	void ShowQuestData();

	UFUNCTION(BlueprintCallable)
	void SelectQuestContent_Slot01();

	UFUNCTION(BlueprintCallable)
	void SelectQuestContent_Slot02();

	UFUNCTION(BlueprintCallable)
	void SelectQuestContent_Slot03();

	UFUNCTION(BlueprintCallable)
	void SelectQuestContent_Slot04();

	UFUNCTION(BlueprintCallable)
	void SelectQuestContent_Slot05();

	void ShowSelectedQuestInfo(int32 Number);

};
