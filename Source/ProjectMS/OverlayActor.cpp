// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlayActor.h"
#include "Components/SceneCaptureComponent2D.h"

// Sets default values
AOverlayActor::AOverlayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	SceneCaptureComp2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComp2D"));
	SceneCaptureComp2D->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AOverlayActor::BeginPlay()
{
	Super::BeginPlay();

	SceneCaptureComp2D->ShowOnlyActorComponents(this);
}

// Called every frame
void AOverlayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Display, TEXT("OverlayActor.."));
}

