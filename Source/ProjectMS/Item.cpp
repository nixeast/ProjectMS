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
	// UStaticMeshComponent를 생성하여 ItemMesh 에 주소를 저장합니다.
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	// ItemMesh를 루트컴포넌트로 지정합니다.
	SetRootComponent(ItemMesh);
	// 박스형 충돌체 UBoxComponent를 생성하고 ItemCollisionBox 에 주소를 저장합니다.
	ItemCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	// 컴포넌트를 루트컴포넌트의 하위에 붙여줍니다.
	ItemCollisionBox->SetupAttachment(RootComponent);
	// 충돌체가 가지고 있는 OnComponentBeginOverlap 이벤트 발생 시 추가로 호출될 함수를 추가합니다.
	ItemCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AItem::ItemBeginOverlap);
	// 충돌체가 가지고 있는 OnComponentEndOverlap 이벤트 발생 시 추가로 호출될 함수를 추가합니다.
	ItemCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AItem::ItemEndOverlap);

	// 텍스트렌더러 컴포넌트를 생성하고 TextRenderComp 에 주소를 저장합니다.
	TextRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	// 컴포넌트를 루트컴포넌트의 하위에 붙여줍니다.
	TextRenderComp->SetupAttachment(RootComponent);

	// 정면 방향을 가르키는 애로우 컴포넌트를 생성하여 ArrowComponent에 저장합니다.
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	// 컴포넌트를 루트컴포넌트의 하위에 붙여줍니다.
	ArrowComponent->SetupAttachment(RootComponent);
	// 아이템 획득가능 표시 아이콘을 그려줄 빌보드컴포넌트를 생성하여 BillboardComponent에 저장합니다.
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	// 컴포넌트를 루트컴포넌트의 하위에 붙여줍니다.
	BillboardComponent->SetupAttachment(RootComponent);
	
	// 플레이어 접촉상태를 false로 초기화합니다.
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

// by정훈, 아이템의 충돌박스에 닿기 시작하면 호출되는 함수입니다.
void AItem::ItemBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 충돌된 대상이 플레이어 캐릭터가 아니면 동작하지 않습니다.
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		// 정상적인 오버랩 시작 이벤트 발생 체크용 로그출력 함수입니다.
		UE_LOG(LogTemp, Display, TEXT("ItemCollisionBox Overlapped.."));
		
		//플레이어캐릭터가 들어옴 상태를 저장합니다.
		IsPlayerIn = true;

		// 충돌한 플레이어캐릭터의 수집타켓의 포인터변수에 현재아이템을 저장합니다.
		Cast<AProjectMSCharacter>(OtherActor)->CurrentHarvestTarget = this;

		// 플레이어 캐릭터의 포커스 타겟을 현재 아이템으로 저장합니다.
		SetPlayerFocusOnThisItem();

		// 아이템 인스턴스에 종속된 빌보드를 켭니다.
		BillboardComponent->SetVisibility(true);
	}
}

void AItem::HideTextRenderComponent()
{
	TextRenderComp->SetVisibility(false);
}

// by정훈, 아이템의 충돌박스에서 벗어나면 호출되는 함수입니다.
void AItem::ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 충돌된 대상이 플레이어 캐릭터가 아니면 동작하지 않습니다.
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		// 정상적인 오버랩 종료 이벤트 발생 체크용 로그출력 함수입니다.
		UE_LOG(LogTemp, Display, TEXT("ItemCollisionBox Overlap ended.."));
		
		//플레이어캐릭터가 나감 상태를 저장합니다.
		IsPlayerIn = false;

		// 충돌한 플레이어캐릭터의 수집타켓의 포인터변수를 nullptr로 저장합니다.
		Cast<AProjectMSCharacter>(OtherActor)->CurrentHarvestTarget = nullptr;

		// 플레이어 캐릭터의 포커스 타겟을 nullptr로 저장합니다.
		PlayerCharacter->ResetFocusedItem();

		// 아이템 인스턴스에 종속된 빌보드를 끕니다.
		BillboardComponent->SetVisibility(false);
	}
}

void AItem::SetPlayerFocusOnThisItem()
{
	PlayerCharacter->SetFocusedItem(this);
}

void AItem::ItemHudScaler()
{
	// by정훈, 저장할 Scale값 = (최대Scale값 - 현재 거리에 따라 줄여줄 Scale값)
	NewScale3DParam = (1.0f - ((DistToCharacter - 100.0f) / 10.0f * 0.03f));

	// by정훈, 저장할 Scale값을 Vector에 저장합니다.
	NewScale3D.X = NewScale3DParam;
	NewScale3D.Y = NewScale3DParam;
	NewScale3D.Z = NewScale3DParam;

	// by정훈, 변경된 Vector값을 빌보드의 Sclae에 입력하여줍니다.
	BillboardComponent->SetRelativeScale3D(NewScale3D);
}

void AItem::CalculateDistToCharacter()
{
	// by정훈, PlayerCharacter가 nullptr이 아닌경우 실행합니다.
	if (PlayerCharacter != nullptr)
	{
		// by정훈, 이 아이템과 플레이어 캐릭터와의 거리를 계산합니다.
		// 계산된 거리의 값을 DistToCharacter에 저장합니다.
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

