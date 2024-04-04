// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CustomDataTables.generated.h"

// by정훈, 퀘스트 데이터 테이블 행의 정보를 담을 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 퀘스트 고유 번호
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 Quest_Order;
	// 퀘스트명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Quest_Name;
	// 퀘스트 클라이언트 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Quest_ClientName;
	// 퀘스트 내용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Quest_Content;
	// 퀘스트 목표물 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Objective_Name;
	// 퀘스트 목표물 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 Objective_Count;
	// 보상 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Reward_Name;
	// 보상 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 Reward_Count;
	// 퀘스트 성공여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool Quest_Success;

};

// by정훈, 대화문 데이터 데이블 행 정보를 담을 구조체입니다.
USTRUCT(BlueprintType)
struct FDialogueTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 대화 상호작용 오브젝트 번호
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	int32 Object_Number;
	// 대화 상호작용 오브젝트 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString Object_Name;
	// 해당하는 대화문
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
