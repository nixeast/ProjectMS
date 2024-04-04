// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMS_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	/*class UTextBlock* text_LoadedAmmo;
	class UTextBlock* text_MagazineCapacity;
	class UTextBlock* text_LeftAmmo;*/

};
