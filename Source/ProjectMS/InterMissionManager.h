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
	
	//by정훈, 인터미션 위젯 생성에 사용될 TSubclassOf 타입 변수를 만들어줌
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> InterMissionMenuClass;

	//by정훈, 생성할 인터미션UI을 가리킬 포인터 변수
	UUserWidget* InterMissionMenu;

	//by정훈, 인터미션UI를 생성함
	void CreateInterMissionMenu();
	
};
