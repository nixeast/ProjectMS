// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CustomDataTables.generated.h"

// by����, ����Ʈ ������ ���̺� ���� ������ ���� ����ü�Դϴ�.
USTRUCT(BlueprintType)
struct FQuestTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// ����Ʈ ���� ��ȣ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 Quest_Order;
	// ����Ʈ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Quest_Name;
	// ����Ʈ Ŭ���̾�Ʈ �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Quest_ClientName;
	// ����Ʈ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Quest_Content;
	// ����Ʈ ��ǥ�� �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Objective_Name;
	// ����Ʈ ��ǥ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 Objective_Count;
	// ���� �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Reward_Name;
	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 Reward_Count;
	// ����Ʈ ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool Quest_Success;

};

// by����, ��ȭ�� ������ ���̺� �� ������ ���� ����ü�Դϴ�.
USTRUCT(BlueprintType)
struct FDialogueTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// ��ȭ ��ȣ�ۿ� ������Ʈ ��ȣ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	int32 Object_Number;
	// ��ȭ ��ȣ�ۿ� ������Ʈ �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString Object_Name;
	// �ش��ϴ� ��ȭ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString Description;

};

UCLASS()
class PROJECTMS_API ACustomDataTables : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomDataTables();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
