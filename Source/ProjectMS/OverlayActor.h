// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OverlayActor.generated.h"

UCLASS()
class PROJECTMS_API AOverlayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOverlayActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(EditAnywhere)
	class USceneCaptureComponent2D* SceneCaptureComp2D;

};
