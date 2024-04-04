// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gate.generated.h"

UCLASS()
class PROJECTMS_API AGate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* CollisionBox;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* GateMesh_Left;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* GateMesh_Right;
	UPROPERTY(EditDefaultsOnly)
	class UTextRenderComponent* TextRenderComp;
	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere)
	class ACameraDirector* CurrentCamDirector;

	UFUNCTION()
	void Collision_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Collision_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void RotateGate_Left();
	void RotateGate_Right();

	void SlideGate_Left();
	void SlideGate_Right();

	UPROPERTY(EditAnywhere, Category = "RotateGate")
	float RotateAmount_LeftGate = -90.0f;
	UPROPERTY(EditAnywhere, Category = "RotateGate")
	float RotateAmount_RightGate = 90.0f;
	UPROPERTY(EditAnywhere, Category = "RotateGate")
	float RotateTime = 3.0f;

	float RotateInDeltaTime;

	UPROPERTY(EditAnywhere, Category="SlideGate")
	float SlideAmount_LeftGate = 150.0f;
	UPROPERTY(EditAnywhere, Category = "SlideGate")
	float SlideAmount_RightGate = -150.0f;
	UPROPERTY(EditAnywhere, Category = "SlideGate")
	float SlideTime = 3.0f;
	
	float SlideInDeltaTime;

	void TimeCalculate();
	float CurrentTime = 0;

	UPROPERTY(EditAnywhere)
	bool ShouldRotateGate;

	UPROPERTY(EditAnywhere)
	bool ShouldSlideGate;

	UPROPERTY(EditAnywhere)
	bool ShouldChangePlayerViewportCam;

	UPROPERTY(EditAnywhere)
	bool IsPlayerHasKey;

	bool CheckPlayerHasKey(AActor* TargetActor);
	
	void MoveViewPortCam_GateKey();

	void MoveViewPortCam_Player();

	FTimerHandle GateTimerHandle;

	bool isHintPlayed;

	UPROPERTY(EditAnywhere)
	class USoundBase* Sfx_GateOpen;

public:

	void ActiveGate(bool HasKey);
};
