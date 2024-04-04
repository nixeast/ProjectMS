// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemButton.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMS_API UInventoryItemButton : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

private:
	int32 CurrentItemNumber;

	UPROPERTY(EditDefaultsOnly)
	int32 ItemCount;
	
	FString ItemName;
	FString ItemInformation;

	void InitCurrentItem();

	class UButton* MyButton;

	class AInGameManager* CurrentInGameManager;

public:
	int32 GetCurrentItemNumber();
	void SetCurrentItemNumber(int32 ItemNumber);

	int32 GetCurrentItemCount();
	void AddCurrentItemCount(int32 AddNumber);
	void DecreaseCurrentItemCount(int32 DecreaseNumber);

	void SetCurrentItemName(FString& Name);
	FString GetCurrentItemName();

	void SetCurrentItemInfo(FString& Info);
	FString GetCurrentItemInfo();

	class UButton* GetMyButton();
	void SetCurrentInGameManager(class AInGameManager* InGameManager);

	UFUNCTION(BlueprintCallable)
	void SetMyButtonToSelectedButton();
};
