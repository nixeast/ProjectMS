// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exit.generated.h"

UCLASS()
class PROJECTMS_API AExit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* ExitCollisionBox;

	UPROPERTY(EditDefaultsOnly)
	class UTextRenderComponent* TextRenderComp;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* ArrowComponent;

	UFUNCTION()
	void ExitBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ExitEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	class AProjectMSCharacter* PlayerCharacter;

	FTimerHandle ExitTimerHandle;

	void TransferCharacterToIntermission();

	float LeftExitTime;
	int32 LastSavedExitTime;

	void CallResultScreen();
};
