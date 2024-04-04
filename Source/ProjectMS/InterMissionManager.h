// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InterMissionManager.generated.h"

UCLASS()
class PROJECTMS_API AInterMissionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInterMissionManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

private:
	
	//by����, ���͹̼� ���� ������ ���� TSubclassOf Ÿ�� ������ �������
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> InterMissionMenuClass;

	//by����, ������ ���͹̼�UI�� ����ų ������ ����
	UUserWidget* InterMissionMenu;

	//by����, ���͹̼�UI�� ������
	void CreateInterMissionMenu();
	
};
