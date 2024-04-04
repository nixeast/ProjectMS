// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemButton.h"
#include "Components/Button.h"
#include "InGameManager.h"

void UInventoryItemButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// by정훈, 현재 아이템버튼을 초기화 합니다.
	InitCurrentItem();
}

void UInventoryItemButton::InitCurrentItem()
{
	// 해당 아이템의 고유 번호를 초기화 합니다.
	CurrentItemNumber = 0;
	// 해당 아이템의 보유 개수를 초기화 합니다.
	ItemCount = 0;
	// 해당 아이템의 이름을 초기화 합니다.
	ItemName = TEXT("None");
	// 해당 아이템의 설명 내용을 초기화 합니다.
	ItemInformation = TEXT("None");
	// MyButton 변수에 현재 위젯 블루프린트 안의 Button_Item 버튼의 주소를 저장합니다.
	MyButton = Cast<UButton>(this->GetWidgetFromName(TEXT("Button_Item")));
	// 버튼이 클릭될 경우 호출될 함수를 추가합니다.
	MyButton->OnClicked.AddDynamic(this, &UInventoryItemButton::SetMyButtonToSelectedButton);
}

int32 UInventoryItemButton::GetCurrentItemNumber()
{
	return CurrentItemNumber;
}

void UInventoryItemButton::SetCurrentItemNumber(int32 ItemNumber)
{
	CurrentItemNumber = ItemNumber;
}

int32 UInventoryItemButton::GetCurrentItemCount()
{
	return ItemCount;
}

void UInventoryItemButton::AddCurrentItemCount(int32 AddNumber)
{
	ItemCount += AddNumber;
	UE_LOG(LogTemp, Display, TEXT("ItemNumber:%d, ItemCount:%d"), CurrentItemNumber, ItemCount);
}

void UInventoryItemButton::DecreaseCurrentItemCount(int32 DecreaseNumber)
{
	ItemCount -= DecreaseNumber;
}

void UInventoryItemButton::SetCurrentItemName(FString& Name)
{
	ItemName = Name;
}

FString UInventoryItemButton::GetCurrentItemName()
{
	return ItemName;
}

void UInventoryItemButton::SetCurrentItemInfo(FString& Info)
{
	ItemInformation = Info;
}

FString UInventoryItemButton::GetCurrentItemInfo()
{
	return ItemInformation;
}

UButton* UInventoryItemButton::GetMyButton()
{
	return MyButton;
}

void UInventoryItemButton::SetCurrentInGameManager(AInGameManager* InGameManager)
{
	CurrentInGameManager = InGameManager;
}

// by정훈, 인게임 매니저에서 현재 선택된 버튼주소를 현재버튼으로 적용시켜줍니다.
void UInventoryItemButton::SetMyButtonToSelectedButton()
{
	CurrentInGameManager->CurrentSelectedButton = this;
	CurrentInGameManager->ShowSelectedItemButtonInfo();
}
