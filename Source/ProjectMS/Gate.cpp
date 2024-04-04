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

// by����, ����Ʈ�� ���� ���� ȸ���Ͽ� ���� �Լ��Դϴ�.
void AGate::RotateGate_Left()
{
	// ƽ�� ȸ�� �� = (�ִ�ȸ���� / ȸ���ð�(��)) * ���� ��ŸŸ��
	RotateInDeltaTime = (RotateAmount_LeftGate / RotateTime) 
		* GetWorld()->DeltaTimeSeconds;
	// ƽ�� ȸ���� ��ŭ �ش�������� ȸ�������ݴϴ�.
	GateMesh_Left->AddLocalRotation(FRotator(0, RotateInDeltaTime, 0));
}

// by����, ����Ʈ�� ������ ���� ȸ���Ͽ� ���� �Լ��Դϴ�.
void AGate::RotateGate_Right()
{
	// ƽ�� ȸ�� �� = (�ִ�ȸ���� / ȸ���ð�(��)) * ���� ��ŸŸ��
	RotateInDeltaTime = (RotateAmount_RightGate / RotateTime) 
		* GetWorld()->DeltaTimeSeconds;
	// ƽ�� ȸ���� ��ŭ �ش�������� ȸ�������ݴϴ�.
	GateMesh_Right->AddLocalRotation(FRotator(0, RotateInDeltaTime, 0));
}

// by����, ����Ʈ�� ���� �����̵�� ���� �Լ��Դϴ�.
void AGate::SlideGate_Left()
{
	// ƽ�� �����̵� �Ÿ� = (�����̵� �Ÿ� / �����̵�ð�(��)) * ���� ��ŸŸ��
	SlideInDeltaTime = (SlideAmount_LeftGate / SlideTime) * GetWorld()->DeltaTimeSeconds;
	// ƽ�� �����̵� �Ÿ���ŭ �ش�������� �̵������ݴϴ�.
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
		//by����, 3���� HideUI_ItemGet �Լ��� �ѹ� ȣ���ϵ��� �մϴ�.
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

