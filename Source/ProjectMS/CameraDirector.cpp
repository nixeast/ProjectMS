// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDirector.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectMSCharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentPlayerCamera = Cast<AProjectMSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->GetFollowCamera();

	if (GateKeyCameraActor != nullptr)
	{
		GateKeyCamera = Cast<UCameraComponent>(GateKeyCameraActor->GetComponentByClass(UCameraComponent::StaticClass()));
	}

	
}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACameraDirector::SwitchViewportToGateKeyCam()
{
	
	if (GateKeyCamera != nullptr)
	{
		
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(GateKeyCameraActor, 3.0f);
		UGameplayStatics::GetPlayerCharacter(this, 0)->EndViewTarget(GetWorld()->GetFirstPlayerController());
	}
}

void ACameraDirector::SwitchViewportToPlayerCam()
{
	if (CurrentPlayerCamera != nullptr && CurrentPlayerCamera->GetOwner() != nullptr)
	{
		if (UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) != nullptr)
		{
			GetWorld()->GetFirstPlayerController()->SetViewTarget(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Can't find player characer.."));
		}

	}
	
	//UGameplayStatics::GetPlayerCharacter(this, 0)->BecomeViewTarget(GetWorld()->GetFirstPlayerController());
}

