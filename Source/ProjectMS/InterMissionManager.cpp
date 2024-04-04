// Fill out your copyright notice in the Description page of Project Settings.


#include "InterMissionManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"

// Sets default values
AInterMissionManager::AInterMissionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInterMissionManager::BeginPlay()
{
	Super::BeginPlay();

	CreateInterMissionMenu();
}

void AInterMissionManager::CreateInterMissionMenu()
{
	// by정훈, TSubclass 타입 변수 InterMissionMenuClass을 이용하여 위젯을 생성합니다. 
	// 인스턴스의 주소를 InterMissionMenu에 담습니다.
	InterMissionMenu = CreateWidget(GetWorld(), InterMissionMenuClass);

	// by정훈, InterMissionMenu 포인터 변수가 널포인터를 가르키지 않은 경우 실행 합니다.
	if (InterMissionMenu != nullptr)
	{
		// by정훈, 인터미션UI위젯에 접근하여 뷰포트 화면에 추가되도록 합니다.
		InterMissionMenu->AddToViewport();
		// by정훈, 인터미션UI위젯을 보이기 상태로 설정해줍니다.
		InterMissionMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

// Called every frame
//void AInterMissionManager::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

