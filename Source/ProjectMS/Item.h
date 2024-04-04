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

	// by����, �÷��̾� ĳ���� �浹 ������ �����մϴ�.
	bool IsPlayerIn;
	// by����, �����۰� �÷��̾���� �Ÿ����� �����մϴ�.
	float DistToCharacter;
	// by����, �����۰� �÷��̾���� �Ÿ����� ���� ���� scale���� �����մϴ�.
	float NewScale3DParam;
	// by����, ���� �� scale���� ������ Vector�� �Դϴ�.
	FVector NewScale3D;
	
	// by����, �÷��̾� ĳ���Ϳ��� �Ÿ������� �������� scale�� �����մϴ�.
	void ItemHudScaler();
	// by����, �����۰� �÷��̾���� �Ÿ��� ����մϴ�.
	void CalculateDistToCharacter();

public:
	void DestroyThisItem();
	int32 GetItemNumber();

	int32 GetCurrentItemAmount();
};
