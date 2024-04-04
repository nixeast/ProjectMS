// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class PROJECTMS_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* ItemCollisionBox;

	UPROPERTY(EditDefaultsOnly)
	class UTextRenderComponent* TextRenderComp;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* ArrowComponent;

	UPROPERTY(EditDefaultsOnly)
	class UBillboardComponent* BillboardComponent;

	UFUNCTION()
	void ItemBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void ShowTextRenderComponent();

	void HideTextRenderComponent();

	class AProjectMSCharacter* PlayerCharacter;

	void SetPlayerFocusOnThisItem();

	UPROPERTY(EditDefaultsOnly)
	int32 ItemNumber;

	UPROPERTY(EditDefaultsOnly)
	int32 ItemAmount;

	// by정훈, 플레이어 캐릭터 충돌 유무를 저장합니다.
	bool IsPlayerIn;
	// by정훈, 아이템과 플레이어와의 거리값을 저장합니다.
	float DistToCharacter;
	// by정훈, 아이템과 플레이어와의 거리값에 따라 계산된 scale값을 저장합니다.
	float NewScale3DParam;
	// by정훈, 수정 된 scale값을 저장할 Vector값 입니다.
	FVector NewScale3D;
	
	// by정훈, 플레이어 캐릭터와의 거리에따라 빌보드의 scale을 조정합니다.
	void ItemHudScaler();
	// by정훈, 아이템과 플레이어와의 거리를 계산합니다.
	void CalculateDistToCharacter();

public:
	void DestroyThisItem();
	int32 GetItemNumber();

	int32 GetCurrentItemAmount();
};
