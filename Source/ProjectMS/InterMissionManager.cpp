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
	// by����, TSubclass Ÿ�� ���� InterMissionMenuClass�� �̿��Ͽ� ������ �����մϴ�. 
	// �ν��Ͻ��� �ּҸ� InterMissionMenu�� ����ϴ�.
	InterMissionMenu = CreateWidget(GetWorld(), InterMissionMenuClass);

	// by����, InterMissionMenu ������ ������ �������͸� ����Ű�� ���� ��� ���� �մϴ�.
	if (InterMissionMenu != nullptr)
	{
		// by����, ���͹̼�UI������ �����Ͽ� ����Ʈ ȭ�鿡 �߰��ǵ��� �մϴ�.
		InterMissionMenu->AddToViewport();
		// by����, ���͹̼�UI������ ���̱� ���·� �������ݴϴ�.
		InterMissionMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

// Called every frame
//void AInterMissionManager::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

