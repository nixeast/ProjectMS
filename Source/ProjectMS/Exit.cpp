// Fill out your copyright notice in the Description page of Project Settings.


#include "Exit.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/ArrowComponent.h"
#include "ProjectMSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "InGameManager.h"

// Sets default values
AExit::AExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExitCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(ExitCollisionBox);
	ExitCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AExit::ExitBeginOverlap);
	ExitCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AExit::ExitEndOverlap);

	TextRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	TextRenderComp->SetupAttachment(RootComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AExit::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = Cast<AProjectMSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (PlayerCharacter != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("PlayerCharacter founded.."));
	}

}

// Called every frame
void AExit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetWorld()->GetTimerManager().IsTimerActive(ExitTimerHandle) == true)
	{
		LeftExitTime = GetWorld()->GetTimerManager().GetTimerRemaining(ExitTimerHandle);
		
		if (FMath::CeilToInt32(LeftExitTime) != LastSavedExitTime)
		{
			LastSavedExitTime = FMath::CeilToInt32(LeftExitTime);
			UE_LOG(LogTemp, Display, TEXT("Left time to exit: %d"), FMath::CeilToInt32(LeftExitTime));
		}
	}
}

void AExit::ExitBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Player in.."));

		if (PlayerCharacter->IsCompletedQuest == true)
		{
			CallResultScreen();
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Can't Exit.. Player need to success currentquest.."));
		}
	}
}

void AExit::CallResultScreen()
{
	PlayerCharacter->GetCurrentIngameManager()->ShowInGameUI();
	PlayerCharacter->GetCurrentIngameManager()->ShowResultPanel();
	PlayerCharacter->SetResultModeControl();
}

void AExit::ExitEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Player out.."));
	}
}

void AExit::TransferCharacterToIntermission()
{
	UGameplayStatics::OpenLevel(this, FName("InterMission"));
}


