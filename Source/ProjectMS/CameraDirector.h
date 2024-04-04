// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class PROJECTMS_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* CurrentPlayerCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AActor* GateKeyCameraActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* GateKeyCamera;

	void SwitchViewportToGateKeyCam();
	void SwitchViewportToPlayerCam();

	bool ReadyToSwitchCam;
};
