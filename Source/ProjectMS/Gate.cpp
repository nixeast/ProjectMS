// Fill out your copyright notice in the Description page of Project Settings.


#include "Gate.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/ArrowComponent.h"
#include "CameraDirector.h"
#include "ProjectMSCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGate::AGate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AGate::Collision_BeginOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AGate::Collision_EndOverlap);

	GateMesh_Left = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh_Left"));
	GateMesh_Left->SetupAttachment(RootComponent);

	GateMesh_Right = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh_Right"));
	GateMesh_Right->SetupAttachment(RootComponent);

	TextRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	TextRenderComp->SetupAttachment(RootComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShouldRotateGate == true)
	{
		if (GateMesh_Left != nullptr)
		{
			RotateGate_Left();
		}

		if (GateMesh_Right != nullptr)
		{
			RotateGate_Right();
		}
		
		TimeCalculate();
	}

	if (ShouldSlideGate == true)
	{
		if (GateMesh_Left != nullptr)
		{
			SlideGate_Left();
		}

		if (GateMesh_Right != nullptr)
		{
			SlideGate_Right();
		}

		TimeCalculate();
	}
	
}

void AGate::Collision_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (this->ActorHasTag(TEXT("BuildingDoor")))
	{
		UE_LOG(LogTemp, Display, TEXT("Contact building door.."));
	}

	if (Cast<AProjectMSCharacter>(OtherActor) == nullptr)
	{
		return;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Player in.."));

		if (Cast<AProjectMSCharacter>(OtherActor)->CurrentGate == nullptr)
		{
			Cast<AProjectMSCharacter>(OtherActor)->CurrentGate = this;
		}
	}
}

void AGate::Collision_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (ShouldChangePlayerViewportCam == false)
	{
		return;
	}*/

	if (Cast<AProjectMSCharacter>(OtherActor) == nullptr)
	{
		return;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Player out.."));

		if (Cast<AProjectMSCharacter>(OtherActor)->CurrentGate != nullptr)
		{
			Cast<AProjectMSCharacter>(OtherActor)->CurrentGate = nullptr;
		}

		/*if (Cast<AProjectMSCharacter>(OtherActor)->CanActiveInteractionObj == true)
		{
			Cast<AProjectMSCharacter>(OtherActor)->CanActiveInteractionObj = false;
		}*/
	}

	/*if (CurrentCamDirector != nullptr)
	{
		if (CurrentCamDirector->ReadyToSwitchCam == true)
		{
			UE_LOG(LogTemp, Display, TEXT("Collision Exit.."));
			CurrentCamDirector->ReadyToSwitchCam = false;
			CurrentCamDirector->SwitchViewportToPlayerCam();
		}
	}*/
}

// by정훈, 게이트의 왼쪽 문을 회전하여 여는 함수입니다.
void AGate::RotateGate_Left()
{
	// 틱당 회전 값 = (최대회전값 / 회전시간(초)) * 월드 델타타임
	RotateInDeltaTime = (RotateAmount_LeftGate / RotateTime) 
		* GetWorld()->DeltaTimeSeconds;
	// 틱당 회전값 만큼 해당방향으로 회전시켜줍니다.
	GateMesh_Left->AddLocalRotation(FRotator(0, RotateInDeltaTime, 0));
}

// by정훈, 게이트의 오른쪽 문을 회전하여 여는 함수입니다.
void AGate::RotateGate_Right()
{
	// 틱당 회전 값 = (최대회전값 / 회전시간(초)) * 월드 델타타임
	RotateInDeltaTime = (RotateAmount_RightGate / RotateTime) 
		* GetWorld()->DeltaTimeSeconds;
	// 틱당 회전값 만큼 해당방향으로 회전시켜줍니다.
	GateMesh_Right->AddLocalRotation(FRotator(0, RotateInDeltaTime, 0));
}

// by정훈, 게이트의 문을 슬라이드로 여는 함수입니다.
void AGate::SlideGate_Left()
{
	// 틱당 슬라이드 거리 = (슬라이드 거리 / 슬라이드시간(초)) * 월드 델타타임
	SlideInDeltaTime = (SlideAmount_LeftGate / SlideTime) * GetWorld()->DeltaTimeSeconds;
	// 틱당 슬라이드 거리만큼 해당방향으로 이동시켜줍니다.
	GateMesh_Left->AddLocalOffset(FVector(0, SlideInDeltaTime, 0));
}

void AGate::SlideGate_Right()
{
	SlideInDeltaTime = (SlideAmount_RightGate / SlideTime) * GetWorld()->DeltaTimeSeconds;

	GateMesh_Right->AddLocalOffset(FVector(0, SlideInDeltaTime, 0));
}

void AGate::TimeCalculate()
{
	CurrentTime = CurrentTime + GetWorld()->DeltaTimeSeconds;
	
	if (CurrentTime >= RotateTime)
	{
		ShouldRotateGate = false;
		ShouldSlideGate = false;
	}

	UE_LOG(LogTemp, Display, TEXT("GateAnimTime:%f"), CurrentTime);
}

void AGate::MoveViewPortCam_GateKey()
{
	if (CurrentCamDirector != nullptr)
	{
		if (CurrentCamDirector->ReadyToSwitchCam == false)
		{
			UE_LOG(LogTemp, Display, TEXT("Collision Enter.."));
			CurrentCamDirector->ReadyToSwitchCam = true;
			CurrentCamDirector->SwitchViewportToGateKeyCam();
		}
	}

	isHintPlayed = true;

	if (GetWorld()->GetTimerManager().IsTimerActive(GateTimerHandle) == false)
	{
		//by정훈, 3초후 HideUI_ItemGet 함수를 한번 호출하도록 합니다.
		GetWorld()->GetTimerManager().SetTimer(GateTimerHandle, this, &AGate::MoveViewPortCam_Player, 4.0f, false);
	}

}

void AGate::ActiveGate(bool HasKey)
{

	if (this->ActorHasTag(TEXT("BuildingDoor")))
	{
		ShouldRotateGate = true;
		return;
	}
	else if (this->ActorHasTag(TEXT("Elevator")))
	{
		ShouldSlideGate = true;
		return;
	}
	else
	{
		if (HasKey == true)
		{
			ShouldRotateGate = true;
		
			if (Sfx_GateOpen != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, Sfx_GateOpen, this->GetActorLocation());
			}
		}
		else if(HasKey == false)
		{

			if (isHintPlayed == true)
			{
				return;
			}

			MoveViewPortCam_GateKey();
		}
	}

}

void AGate::MoveViewPortCam_Player()
{
	if (CurrentCamDirector != nullptr)
	{
		if (CurrentCamDirector->ReadyToSwitchCam == true)
		{
			UE_LOG(LogTemp, Display, TEXT("Collision Exit.."));
			CurrentCamDirector->ReadyToSwitchCam = false;
			CurrentCamDirector->SwitchViewportToPlayerCam();
		}
	}
}

bool AGate::CheckPlayerHasKey(AActor* TargetActor)
{
	if (Cast<AProjectMSCharacter>(TargetActor)->HasGateKey == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

