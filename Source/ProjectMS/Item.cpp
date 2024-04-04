// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/PrimitiveComponent.h"
#include "ProjectMSCharacter.h"
#include "Components/BillboardComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// UStaticMeshComponent�� �����Ͽ� ItemMesh �� �ּҸ� �����մϴ�.
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	// ItemMesh�� ��Ʈ������Ʈ�� �����մϴ�.
	SetRootComponent(ItemMesh);
	// �ڽ��� �浹ü UBoxComponent�� �����ϰ� ItemCollisionBox �� �ּҸ� �����մϴ�.
	ItemCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	// ������Ʈ�� ��Ʈ������Ʈ�� ������ �ٿ��ݴϴ�.
	ItemCollisionBox->SetupAttachment(RootComponent);
	// �浹ü�� ������ �ִ� OnComponentBeginOverlap �̺�Ʈ �߻� �� �߰��� ȣ��� �Լ��� �߰��մϴ�.
	ItemCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AItem::ItemBeginOverlap);
	// �浹ü�� ������ �ִ� OnComponentEndOverlap �̺�Ʈ �߻� �� �߰��� ȣ��� �Լ��� �߰��մϴ�.
	ItemCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AItem::ItemEndOverlap);

	// �ؽ�Ʈ������ ������Ʈ�� �����ϰ� TextRenderComp �� �ּҸ� �����մϴ�.
	TextRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	// ������Ʈ�� ��Ʈ������Ʈ�� ������ �ٿ��ݴϴ�.
	TextRenderComp->SetupAttachment(RootComponent);

	// ���� ������ ����Ű�� �ַο� ������Ʈ�� �����Ͽ� ArrowComponent�� �����մϴ�.
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	// ������Ʈ�� ��Ʈ������Ʈ�� ������ �ٿ��ݴϴ�.
	ArrowComponent->SetupAttachment(RootComponent);
	// ������ ȹ�氡�� ǥ�� �������� �׷��� ������������Ʈ�� �����Ͽ� BillboardComponent�� �����մϴ�.
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	// ������Ʈ�� ��Ʈ������Ʈ�� ������ �ٿ��ݴϴ�.
	BillboardComponent->SetupAttachment(RootComponent);
	
	// �÷��̾� ���˻��¸� false�� �ʱ�ȭ�մϴ�.
	IsPlayerIn = false;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = Cast<AProjectMSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	if (PlayerCharacter != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("PlayerCharacter founded.."));
	}

	BillboardComponent->SetVisibility(false);
	BillboardComponent->SetHiddenInGame(false);

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPlayerIn == true)
	{
		if (PlayerCharacter != nullptr)
		{
			CalculateDistToCharacter();

			ItemHudScaler();

		}
	}
}

void AItem::ShowTextRenderComponent()
{
	TextRenderComp->SetVisibility(true);
}

// by����, �������� �浹�ڽ��� ��� �����ϸ� ȣ��Ǵ� �Լ��Դϴ�.
void AItem::ItemBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �浹�� ����� �÷��̾� ĳ���Ͱ� �ƴϸ� �������� �ʽ��ϴ�.
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		// �������� ������ ���� �̺�Ʈ �߻� üũ�� �α���� �Լ��Դϴ�.
		UE_LOG(LogTemp, Display, TEXT("ItemCollisionBox Overlapped.."));
		
		//�÷��̾�ĳ���Ͱ� ���� ���¸� �����մϴ�.
		IsPlayerIn = true;

		// �浹�� �÷��̾�ĳ������ ����Ÿ���� �����ͺ����� ����������� �����մϴ�.
		Cast<AProjectMSCharacter>(OtherActor)->CurrentHarvestTarget = this;

		// �÷��̾� ĳ������ ��Ŀ�� Ÿ���� ���� ���������� �����մϴ�.
		SetPlayerFocusOnThisItem();

		// ������ �ν��Ͻ��� ���ӵ� �����带 �մϴ�.
		BillboardComponent->SetVisibility(true);
	}
}

void AItem::HideTextRenderComponent()
{
	TextRenderComp->SetVisibility(false);
}

// by����, �������� �浹�ڽ����� ����� ȣ��Ǵ� �Լ��Դϴ�.
void AItem::ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// �浹�� ����� �÷��̾� ĳ���Ͱ� �ƴϸ� �������� �ʽ��ϴ�.
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		// �������� ������ ���� �̺�Ʈ �߻� üũ�� �α���� �Լ��Դϴ�.
		UE_LOG(LogTemp, Display, TEXT("ItemCollisionBox Overlap ended.."));
		
		//�÷��̾�ĳ���Ͱ� ���� ���¸� �����մϴ�.
		IsPlayerIn = false;

		// �浹�� �÷��̾�ĳ������ ����Ÿ���� �����ͺ����� nullptr�� �����մϴ�.
		Cast<AProjectMSCharacter>(OtherActor)->CurrentHarvestTarget = nullptr;

		// �÷��̾� ĳ������ ��Ŀ�� Ÿ���� nullptr�� �����մϴ�.
		PlayerCharacter->ResetFocusedItem();

		// ������ �ν��Ͻ��� ���ӵ� �����带 ���ϴ�.
		BillboardComponent->SetVisibility(false);
	}
}

void AItem::SetPlayerFocusOnThisItem()
{
	PlayerCharacter->SetFocusedItem(this);
}

void AItem::ItemHudScaler()
{
	// by����, ������ Scale�� = (�ִ�Scale�� - ���� �Ÿ��� ���� �ٿ��� Scale��)
	NewScale3DParam = (1.0f - ((DistToCharacter - 100.0f) / 10.0f * 0.03f));

	// by����, ������ Scale���� Vector�� �����մϴ�.
	NewScale3D.X = NewScale3DParam;
	NewScale3D.Y = NewScale3DParam;
	NewScale3D.Z = NewScale3DParam;

	// by����, ����� Vector���� �������� Sclae�� �Է��Ͽ��ݴϴ�.
	BillboardComponent->SetRelativeScale3D(NewScale3D);
}

void AItem::CalculateDistToCharacter()
{
	// by����, PlayerCharacter�� nullptr�� �ƴѰ�� �����մϴ�.
	if (PlayerCharacter != nullptr)
	{
		// by����, �� �����۰� �÷��̾� ĳ���Ϳ��� �Ÿ��� ����մϴ�.
		// ���� �Ÿ��� ���� DistToCharacter�� �����մϴ�.
		DistToCharacter = GetDistanceTo(PlayerCharacter);
	}
}

void AItem::DestroyThisItem()
{
	this->Destroy();
}

int32 AItem::GetItemNumber()
{
	return ItemNumber;
}

int32 AItem::GetCurrentItemAmount()
{
	return ItemAmount;
}

