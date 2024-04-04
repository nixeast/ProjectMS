// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemButton.h"
#include "Components/Button.h"
#include "InGameManager.h"

void UInventoryItemButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// by����, ���� �����۹�ư�� �ʱ�ȭ �մϴ�.
	InitCurrentItem();
}

void UInventoryItemButton::InitCurrentItem()
{
	// �ش� �������� ���� ��ȣ�� �ʱ�ȭ �մϴ�.
	CurrentItemNumber = 0;
	// �ش� �������� ���� ������ �ʱ�ȭ �մϴ�.
	ItemCount = 0;
	// �ش� �������� �̸��� �ʱ�ȭ �մϴ�.
	ItemName = TEXT("None");
	// �ش� �������� ���� ������ �ʱ�ȭ �մϴ�.
	ItemInformation = TEXT("None");
	// MyButton ������ ���� ���� �������Ʈ ���� Button_Item ��ư�� �ּҸ� �����մϴ�.
	MyButton = Cast<UButton>(this->GetWidgetFromName(TEXT("Button_Item")));
	// ��ư�� Ŭ���� ��� ȣ��� �Լ��� �߰��մϴ�.
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

// by����, �ΰ��� �Ŵ������� ���� ���õ� ��ư�ּҸ� �����ư���� ��������ݴϴ�.
void UInventoryItemButton::SetMyButtonToSelectedButton()
{
	CurrentInGameManager->CurrentSelectedButton = this;
	CurrentInGameManager->ShowSelectedItemButtonInfo();
}
