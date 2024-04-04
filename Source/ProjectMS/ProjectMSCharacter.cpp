// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectMSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Item.h"
#include "InGameManager.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "Gate.h"


//////////////////////////////////////////////////////////////////////////
// AProjectMSCharacter

AProjectMSCharacter::AProjectMSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	MaxHealth = 100.0f;
	CurrentHealth = 100.0f;

	// by정훈, 총기 메시를 할당 할 스태틱메시 컴포넌트를 GunMesh를 생성합니다.
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	// by정훈, GunMesh르 캐릭터 메시의 오른손 소켓에 부착시켜줍니다.
	GunMesh->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	
	MyDamage = 25.0f;
	GunFireStartDist = 50.0f;
	MuzzleSparkStartDist = -50.0;

	// 0 = pistol, 1 = shotgun
	CurrentWeaponNum = 0;

	ShotgunAngle_Min = -25;
	ShotgunAngle_Max = 25;
}

void AProjectMSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	FocusedItem = nullptr;
	ShowMyItemArray();

	FindCurrentInGameManager();

	CurrentHarvestPercent = 0;

	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	if (UGameplayStatics::GetCurrentLevelName(this) == TEXT("InterMission"))
	{
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
	}
	else
	{
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
	}

	IsCompletedQuest = false;

	UE_LOG(LogTemp, Display, TEXT("Current PC health:%f"), CurrentHealth);
}

void AProjectMSCharacter::Tick(float DeltaTime)
{
	if (Aimed == true)
	{
		TraceAimTarget();
	}

	if (HarvestMode == true)
	{
		Harvest();
	}
}

void AProjectMSCharacter::ChangeMovementMode()
{
	if (Aimed == true)
	{
		CurrentMovementMode = 1;
		return;
	}

	CurrentMovementMode = 0;
}

void AProjectMSCharacter::UpdateMovementSpeed()
{
	//Normal
	if (CurrentMovementMode == 0)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed_Origin;
	}
	//Aimed
	else if (CurrentMovementMode == 1)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed_Aim;
	}
}

void AProjectMSCharacter::Interact()
{
	UE_LOG(LogTemp, Display, TEXT("Interacted.."));

	if (FocusedItem == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("No Focused Item.."));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Focused Item:%s"), *FocusedItem->GetName());
		AddMyItemArray(FocusedItem->GetItemNumber());
		FocusedItem->DestroyThisItem();
		FocusedItem = nullptr;
	}

	ShowMyItemArray();
}

void AProjectMSCharacter::ShowMyItemArray()
{

	if (MyItemArray.Num() == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("No item.."));
	}
	else
	{
		for (int i = 0; i < MyItemArray.Num(); i++)
		{
			UE_LOG(LogTemp, Display, TEXT("Item%d :%d"), i, MyItemArray[i]);
		}
	}
}

void AProjectMSCharacter::AddMyItemArray(int32 ItemNumber)
{
	MyItemArray.Emplace(ItemNumber);
}


AActor* AProjectMSCharacter::FindActorByName(FString ActorName)
{
	TArray<AActor*> ActorsInScene{};

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorsInScene);

	if (ActorsInScene.Num() > 0)
	{
		for (int i = 0; i < ActorsInScene.Num(); i++)
		{
			if (ActorsInScene[i]->GetActorLabel() == ActorName)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s founded.."), *ActorName);

				return ActorsInScene[i];
			}
		}
	}

	return nullptr;
}

// by정훈, 조준 시 캐릭터를 회전시키는 함수
void AProjectMSCharacter::SetCharacterOrientationToAim(const FVector& AimTarget)
{
	// 조준대상과 캐릭터와의 벡터값을 구하여 방향으로 변환시킵니다.
	ToAimTarget = AimTarget - GetMesh()->GetComponentLocation();
	ToAimTargetRotation = ToAimTarget.Rotation();
	// 방향값에 추가적인 보정값을 더하여 캐릭터의 RootComponent 방향값을 수정합니다.
	LookAtRotation = FRotator(0, ToAimTargetRotation.Yaw + AdditionalAimYawBalance, 0);
	GetCapsuleComponent()->SetWorldRotation(LookAtRotation);
	
}

// by정훈, 총기 발사 시 호출되는 함수
void AProjectMSCharacter::GunShoot()
{
	// 0=pistol, 1=shotgun
	if (CurrentWeaponNum == 0)
	{
		// by정훈, 조준 상태와 장전된 총알이 0 이상일 경우 발사
		if (Aimed == true && LoadedAmmo > 0)
		{
			// by정훈, 라인트레이스 사용하여 충돌체크
			TraceGunShootTarget();
			// by정훈, 장정된 총알 수를 감소시킴
			ConsumeAmmo();
			// by정훈, 보유중인 총알정보UI 업데이트
			UpdateAmmoUI();
			// by정훈, 보유한 총알이 0일경우 총알아이템 인벤토리에서 제거
			if (LeftAmmo + LoadedAmmo <= 0)
			{
				CurrentInGameManager->DeleteInventoryItemButton();
			}
		}

	}
	else
	{
		// by정훈, 조준 상태와 장전된 총알이 0 이상일 경우 발사
		if (Aimed == true && LoadedAmmo_Shotgun > 0)
		{
			// by정훈, 라인트레이스 사용하여 충돌체크
			TraceGunShootTarget();
			// by정훈, 장정된 총알 수를 감소시킴
			ConsumeAmmo();
			// by정훈, 보유중인 총알정보UI 업데이트
			UpdateAmmoUI();
			// by정훈, 보유한 총알이 0일경우 총알아이템 인벤토리에서 제거
			/*if (LeftAmmo + LoadedAmmo <= 0)
			{
				CurrentInGameManager->DeleteInventoryItemButton();
			}*/
		}
	}

}

void AProjectMSCharacter::SetFocusedItem(AItem* TargetItem)
{
	if (TargetItem != nullptr)
	{
		FocusedItem = TargetItem;
	}
	else
	{
		FocusedItem = nullptr;
	}
}

void AProjectMSCharacter::ResetFocusedItem()
{
	FocusedItem = nullptr;
}



//////////////////////////////////////////////////////////////////////////
// Input

void AProjectMSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectMSCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectMSCharacter::Look);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AProjectMSCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AProjectMSCharacter::Aim_End);

		//Interacting
		EnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Started, this, &AProjectMSCharacter::Interact);

		//GunShooting
		EnhancedInputComponent->BindAction(GunShootAction, ETriggerEvent::Started, this, &AProjectMSCharacter::GunShoot);

		//Harvesting
		EnhancedInputComponent->BindAction(HarvestAction, ETriggerEvent::Started, this, &AProjectMSCharacter::HarvestOn);
		EnhancedInputComponent->BindAction(HarvestAction, ETriggerEvent::Completed, this, &AProjectMSCharacter::HarvestOff);

		//Calling Dialogue
		EnhancedInputComponent->BindAction(HarvestAction, ETriggerEvent::Started, this, &AProjectMSCharacter::CallDialogue);

		//Open unlocked door
		EnhancedInputComponent->BindAction(HarvestAction, ETriggerEvent::Started, this, &AProjectMSCharacter::OpenCurrentGateDoor);

		//Inventorying
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &AProjectMSCharacter::InventorySwitch);

		//Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AProjectMSCharacter::ReloadCurrentWeaponAmmo);
	}

}

void AProjectMSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AProjectMSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProjectMSCharacter::Aim()
{
	CameraBoom->TargetArmLength = CameraboomLength_Aimed;
	Aimed = true;
	ChangeMovementMode();
	UpdateMovementSpeed();
	Anim_AimedOn();
	UE_LOG(LogTemp, Display, TEXT("Aimed.."));

	CurrentInGameManager->ShowUI_AimMode();
	CurrentInGameManager->ShowInGameUI();
}

void AProjectMSCharacter::Aim_End()
{
	CameraBoom->TargetArmLength = CameraboomLength_Origin;
	Aimed = false;
	ChangeMovementMode();
	UpdateMovementSpeed();
	Anim_AimedOff();
	UE_LOG(LogTemp, Display, TEXT("Aim end.."));

	CurrentInGameManager->HideInGameUI();
}


void AProjectMSCharacter::TraceAimTarget()
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerCamLocation, PlayerCamRotation);

	AimTraceStart = PlayerCamLocation + PlayerCamRotation.Vector();
	AimTraceEnd = PlayerCamLocation + PlayerCamRotation.Vector() * 10000.0f;

	bSuccess_Aim = GetWorld()->LineTraceSingleByChannel
	(AimHit, AimTraceStart, AimTraceEnd, ECollisionChannel::ECC_GameTraceChannel1);

	if (bSuccess_Aim == true)
	{
		/*if (AimHit.GetActor() != nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("Hit:%s"), *AimHit.GetActor()->GetName());
		}*/
		AimTraceImpactPoint = AimHit.ImpactPoint + this->GetActorRotation().Vector() * 10.0f;
		SetCharacterOrientationToAim(AimHit.ImpactPoint);
	}
}

void AProjectMSCharacter::TraceGunShootTarget()
{
	// by정훈, 플레이어 카메라 위치와 바라보는 방향값을 가져옵니다.
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerCamLocation, PlayerCamRotation);
	// by정훈, 무기를 들고있는 소켓의 위치값을 이용하여 발사될 총알의 시작지점을 저장합니다.
	GunShootTraceStart = this->GetMesh()->GetSocketLocation(TEXT("hand_rSocket")) 
		+ (this->GetMesh()->GetForwardVector().Rotation().Add(0,90.0,0).Vector() * GunFireStartDist);
	// by정훈, 플레이어 카메라 값을 이용하여 충돌체크 종료 지점의 값을 저장합니다.
	GunShootTraceEnd = PlayerCamLocation + PlayerCamRotation.Vector() * 10000.0f;
	
	if (CurrentWeaponNum == 0)
	{
		// by정훈, 라인트레이스 이용하여 충돌체크 결과값을 저장합니다.
		bSuccess_GunShoot = GetWorld()->LineTraceSingleByChannel(GunShootHit, GunShootTraceStart, AimTraceImpactPoint, ECollisionChannel::ECC_GameTraceChannel2);
		// by정훈, 라인트레이스와 동일한 값으로 시각 체크용 라인을 생성합니다.
		DrawDebugLine(GetWorld(), GunShootTraceStart, AimTraceImpactPoint, FColor::Red, false, 2.0f);
		// by정훈, 총알 발사 효과음을 재생합니다.
		PlayGunShootSFX();
	}
	else
	{
		// 샷건 총알의 분산값을 저장할 변수입니다.
		float DispersePoint = 0;
		// 샷건 총알의 분산 위치값을 저장할 변수입니다.
		FVector DisperseVec = FVector(0, 0, 0);
		// 샷건 총알의 도착지점을 저장할 벡터입니다.
		FVector NewImpactPoint = AimTraceImpactPoint;

		// 샷건 발사 시 발사할 총알의 수를 4개로 정해줍니다.
		for (int32 i = 0; i < 4; i++)
		{
			// 라인트레이스 하여 발사한 총알의 명중 유무를 체크합니다.
			bSuccess_GunShoot = GetWorld()->LineTraceSingleByChannel
			(GunShootHit, GunShootTraceStart, NewImpactPoint, ECollisionChannel::ECC_GameTraceChannel2);
			// 총알 발사의 궤적을 체크하기 위한 용도의 디버그 함수입니다.
			DrawDebugLine(GetWorld(), GunShootTraceStart, NewImpactPoint, FColor::Red, false, 2.0f);
			// 샷건 총알의 분산을 랜덤값으로 저장합니다.
			DispersePoint = (float)FMath::RandRange(ShotgunAngle_Min, ShotgunAngle_Max);
			// 랜덤값으로 정한 분산값을 DisperseVec의 X,Y,Z에 저장합니다.
			DisperseVec.X = DispersePoint;
			DisperseVec.Y = DispersePoint;
			DisperseVec.Z = DispersePoint;
			// 분산값이 입력된 TempVec과 기존의 영점 값인 AimTraceImpactPoint를 더합니다.
			NewImpactPoint = AimTraceImpactPoint + DisperseVec;
		}

		PlayGunShootSFX();
	}

	// by정훈, 충돌체크 되었을 경우 실행합니다.
	if (bSuccess_GunShoot == true)
	{
		// by정훈, 파티클이펙트가 nullptr이 아닌지 체크합니다.
		if (MuzzleParticle != nullptr)
		{
			// by정훈, 캐릭터 메쉬의 오른손소켓 위치를 기준으로 파티클을 생성합니다.
			UGameplayStatics::SpawnEmitterAttached(
				MuzzleParticle, this->GetMesh(), TEXT("hand_rSocket"),
				FVector(MuzzleSparkStartDist,0,10.0), FRotator(90.0, 0, 0), 
				EAttachLocation::KeepRelativeOffset, true);
		}
		// by정훈, 충돌체크 된 대상이 액터인지 체크합니다.
		if (GunShootHit.GetActor() == nullptr)
		{
			return;
		}
		else
		{
			// by정훈, 충돌체크 된 액터의 이름을 출력하여 디버그에 참고합니다.
			UE_LOG(LogTemp, Display, TEXT("Hit:%s"), *GunShootHit.GetActor()->GetName());
			
			// by정훈, 충돌체크 된 액터가 Enemy인지 체크합니다.
			if (Cast<AEnemy>(GunShootHit.GetActor()) != nullptr)
			{
				// by정훈, 대상이 Enemy액터일 경우 데미지를 전달합니다.
				Cast<AEnemy>(GunShootHit.GetActor())->GetDamage(MyDamage);
			}
		}
	}
}

void AProjectMSCharacter::ConsumeAmmo()
{
	if (CurrentWeaponNum == 0)
	{
		LoadedAmmo--;
		CurrentInGameManager->DecreasePistolAmmoButtonCountInfo();
		UE_LOG(LogTemp, Display, TEXT("Current Ammo: %d"), LoadedAmmo);

	}
	else
	{
		LoadedAmmo_Shotgun--;
		//CurrentInGameManager->DecreasePistolAmmoButtonCountInfo();
		UE_LOG(LogTemp, Display, TEXT("Current Shotgun Ammo: %d"), LoadedAmmo_Shotgun);
	}
	
}

void AProjectMSCharacter::UpdateAmmoUI()
{
	CurrentInGameManager->InitPlayerGunAmmo();
}

void AProjectMSCharacter::Harvest()
{
	// by정훈, 아이템 획득 동작 시 포커스된 아이템 유무를 확인합니다.
	if (FocusedItem == nullptr)
	{
		// by정훈, 포커스된 아이템이 없다면 확인용 코드를 출력하고 합수를 종료합니다.
		UE_LOG(LogTemp, Display, TEXT("No Focused Item.."));
	}
	// by 정훈, 포커스 된 아이템이 있을 경우 획득 과정을 시작합니다.
	else
	{
		// by 정훈, 현재 아이템 획득 진행률이 100% 이상이 되는 경우의 조건입니다.
		if (CurrentHarvestPercent >= 10.0f)
		{
			// by 정훈, 현재 획득한 아이템의 고유번호를 배열에 담아 저장하도록 합니다.
			AddMyItemArray(FocusedItem->GetItemNumber());
			// by 정훈, GameManager가 현재 획득한 아이템의 번호를 기억하도록 해 줍니다.
			CurrentInGameManager->SetFocusedItemNumber(FocusedItem->GetItemNumber());
			// by 정훈, 획득한 아이템 표시 UI를 호출합니다.
			CurrentInGameManager->ShowUI_ItemGet();
			// by 정훈, 획득한 아이템 이미지를 슬롯에 출력합니다.
			CurrentInGameManager->SetImage_GetItemInfo();
			// by 정훈, 획득한 아이템 정보 화면을 호출합니다.
			CurrentInGameManager->ShowUI_GetItemInfo();
			SetResultModeControl();
			// by 정훈, 현재 수집한 아이템이 퀘스트 아이템인지 판단하는 함수를 호출합니다.
			MatchQuestItem();
			// by 정훈, 현재 수집한 아이템이 기존에 획득했던 아이템인지 판단하는 함수를 호출합니다.
			bool isMatched = MatchSameItem();
			
			if (isMatched == true)
			{
				// 기존 아이템 수량 추가
				CurrentInGameManager->AddExistItemCount();
			}
			else if(isMatched == false)
			{
				// 아이템 버튼 신규 생성
				CurrentInGameManager->CreateItemButton(FocusedItem->GetItemNumber());
				// 인벤토리 안에 아이템 버튼 배치
				CurrentInGameManager->AddItemtoInventory();

				if (FocusedItem->GetItemNumber() == 3)
				{
					HasGateKey = true;
				}
				else if (FocusedItem->GetItemNumber() == 4)
				{
					HasElevatorCardKey = true;
				}
				else if (FocusedItem->GetItemNumber() == 5)
				{
					// shotgun
					ChangeCurrentWeapon(1);
				}
			}

			CurrentInGameManager->UpdatePlayerGunAmmoUI();
			// by 정훈, 현재 퀘스트를 완료하였는지 판단하는 함수를 호출합니다.
			CurrentInGameManager->CheckQuestComplete();
			// by 정훈, 수확 완료한 아이템 인스턴스를 제거하고, FocusedItem 변수를 nullptr로 지정하여 초기화 해 줍니다.
			FocusedItem->DestroyThisItem();
			FocusedItem = nullptr;
			// by 정훈, 아이템 수확 상태를 종료하는 함수를 호출합니다.
			HarvestOff();
			// by 정훈, 아이템 획득 상태를 UI에 갱신하여 보여줍니다.
			ShowMyItemArray();
		}
		// by 정훈, 아이템 획득 상태가 100% 미만일 경우의 조건입니다.
		else
		{
			// by정훈, 월드의 프레임당 시간단위값을 참조하여 획득진행률을 증가시켜줍니다.
			CurrentHarvestPercent += GetWorld()->DeltaTimeSeconds * 1.0f * 2.0f;
			// by정훈, 아이템의 획득 진행률 값을 아이템 수확UI에 표시하여줍니다.
			// (프레임당 시간값) * (ProgressBar적용계수) * (획득속도증감계수)
			CurrentInGameManager->IncreaseProgressBar_Harvest(GetWorld()->DeltaTimeSeconds * 0.1f * 2.0f);
		}
	}
}

void AProjectMSCharacter::HarvestOn()
{

	if (CurrentHarvestTarget == nullptr)
	{
		return;
	}

	HarvestMode = true;

	if (FocusedItem != nullptr)
	{
		Anim_Harvest();

		CurrentInGameManager->ShowUI_HarvestMode();
		CurrentInGameManager->ShowInGameUI();

		CurrentInGameManager->CheckEmptyInventorySlotNum();
	}
}

void AProjectMSCharacter::HarvestOff()
{
	Anim_Harvest();

	HarvestMode = false;
	CurrentHarvestPercent = 0;
	CurrentInGameManager->HideUI_HarvestMode();
	CurrentInGameManager->ResetProgressBar_Harvest();
}

void AProjectMSCharacter::CallDialogue()
{
	if (CurrentGate != nullptr)
	{
		if (CurrentGate->ActorHasTag(TEXT("BuildingDoor")) == true)
		{
			return;
		}
		else if(CurrentGate->ActorHasTag(TEXT("Elevator")) == true)
		{
			return;
		}
		else
		{
			CurrentInGameManager->ShowUI_Interaction();
			// 인게임 UI를 켭니다.
			CurrentInGameManager->ShowInGameUI();
			// 마우스커서를 보이도록 합니다.
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			// InputModeData의 마우스가 뷰포트에 보이도록 잠급니다.
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			// 설정한 InputModeData를 플레이어의 인풋모드에 적용해줍니다.
			GetWorld()->GetFirstPlayerController()->SetInputMode(InputModeData);
		}
	}
}

void AProjectMSCharacter::OpenCurrentGateDoor()
{
	if (CurrentGate != nullptr)
	{
		if (CurrentGate->ActorHasTag(TEXT("BuildingDoor")) == true)
		{
			CurrentGate->ActiveGate(false);
		}
		else if (CurrentGate->ActorHasTag(TEXT("Elevator")) == true)
		{
			if (HasElevatorCardKey == true)
			{
				CurrentGate->ActiveGate(false);
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("You have no cardkey.."));
			}
		}
		else
		{
			return;
		}
	}
}

// by정훈, 현재 소지하는 무기를 교체합니다.
void AProjectMSCharacter::ChangeCurrentWeapon(int32 weaponNum)
{
	// 현재 무기의 정보를 바꿀 무기의 정보로 저장합니다.
	CurrentWeaponNum = weaponNum;
	
	// 교체할 무기가 권총 일 경우
	if (CurrentWeaponNum == 0)
	{
		// 권총 스태틱 메시로 변경하고 위치값을 조정해 줍니다.
		GunMesh->SetStaticMesh(GunMesh_Shotgun);
		GunMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		GunMesh->SetRelativeLocation(FVector(-10.0f, 3.0f, 1.0f));
		// 권총의 총알 발사 시작지점을 조정해 줍니다.
		GunFireStartDist = 50.0f;
	}
	// 교체할 무기가 샷건일 경우
	else if (CurrentWeaponNum == 1)
	{
		// 샷건 스태틱 메시로 변경하고 위치값을 조정해 줍니다.
		GunMesh->SetStaticMesh(GunMesh_Shotgun);
		GunMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		GunMesh->SetRelativeLocation(FVector(-25.0f, 3.0f, 1.0f));
		// 샷건의 발사 시작지점을 조정해 줍니다.
		GunFireStartDist = 100.0f;
		// 샷건에 장전된 탄약을 늘려줍니다.
		LoadedAmmo_Shotgun = 10;
		// 소지중인 샷건의 탄약을 늘려줍니다.
		Ammo_Shotgun = Ammo_Shotgun + 10;
	}
}

void AProjectMSCharacter::DismissDialogu()
{
	//CurrentInGameManager->HideUI_Interaction();
	
	// 인게임 UI를 끕니다.
	CurrentInGameManager->HideInGameUI();
	
	// 마우스커서를 보이지 않도록 합니다.
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	// InputModeData의 마우스가 뷰포트에 보이도록 잠그지 않습니다.
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// 설정한 InputModeData를 플레이어의 인풋모드에 적용해줍니다.
	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
}

void AProjectMSCharacter::ActiveCurrentGate()
{
	if (CurrentGate != nullptr)
	{
		CurrentGate->ActiveGate(HasGateKey);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Have no CurrentGate.."));
	}
}

void AProjectMSCharacter::Anim_Harvest()
{
	// by정훈, 아이템 수집모드인 경우
	if (HarvestMode == true)
	{
		// by정훈, 수집 애니메이션 트리거를 on 합니다.
		ShouldKneeling = true;
		// by정훈, 캐릭터의 이동 입력을 off 합니다.
		GetCharacterMovement()->DisableMovement();
	}
	// by정훈, 아이템 수집모드가 아닌 경우
	else if(HarvestMode == false)
	{
		//by 정훈, 수집 애니메이션 트리거를 off 합니다.
		ShouldKneeling = false;
		// by정훈, 캐릭터의 이동 입력을 on 합니다.
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AProjectMSCharacter::Anim_AimedOn()
{
	ShouldAim = true;
}

void AProjectMSCharacter::Anim_AimedOff()
{
	ShouldAim = false;
}

void AProjectMSCharacter::PlayGunShootSFX()
{
	if (Sfx_Pistol_Shoot != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sfx_Pistol_Shoot, this->GetActorLocation());
	}
}

void AProjectMSCharacter::DiePlayerCharacter()
{
	// by정훈, 플레이어캐릭터 죽음 애니메이션 실행.
	ShouldDie = true;
}

void AProjectMSCharacter::Anim_ReloadOn()
{
	// by정훈, 애니메이션 블루프린트에서 리로드 애니메이션이 실행되도록 합니다.
	ShouldReload = true;
}



void AProjectMSCharacter::Anim_ReloadOff()
{
	// by정훈, 애니메이션 블루프린트에서 리로드 상태가 종료되도록 합니다.
	ShouldReload = false;
}

int32 AProjectMSCharacter::GetLoadedAmmo()
{
	return LoadedAmmo;
}

int32 AProjectMSCharacter::GetLeftAmmo()
{
	return LeftAmmo;
}

// by정훈, 인벤토리 단축키를 눌러 UI를 호출합니다.
void AProjectMSCharacter::InventorySwitch()
{
	// 인벤토리가 꺼져있을 경우.
	if (InventoryMode == false)
	{
		// 인벤토리모드 UI로 전환합니다.
		CurrentInGameManager->ShowUI_InventoryMode();
		// 인게임 UI를 켭니다.
		CurrentInGameManager->ShowInGameUI();
		// 인벤토리 모드를 on 합니다.
		InventoryMode = true;
		// 마우스커서를 보이도록 합니다.
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		// InputModeData의 마우스가 뷰포트에 보이도록 잠급니다.
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		// 설정한 InputModeData를 플레이어의 인풋모드에 적용해줍니다.
		GetWorld()->GetFirstPlayerController()->SetInputMode(InputModeData);
	}
	// by정훈, 인벤토리가 켜져있을 경우.
	else
	{
		// 인게임 UI를 끕니다.
		CurrentInGameManager->HideInGameUI();
		// 인벤토리모드를 off합니다.
		InventoryMode = false;
		// 마우스커서를 보이지 않도록 합니다.
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
		// InputModeData의 마우스가 뷰포트에 보이도록 잠그지 않습니다.
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		// 설정한 InputModeData를 플레이어의 인풋모드에 적용해줍니다.
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
	}
}

int32 AProjectMSCharacter::GetMagazineCapacity()
{
	return MagazineCapacity;
}

// by정훈, 장착중인 무기의 탄창을 재충전합니다.
void AProjectMSCharacter::ReloadCurrentWeaponAmmo()
{
	int32 CountToAddAmmo = 0;
	CountToAddAmmo = MagazineCapacity - LoadedAmmo;
	// by정훈, 인벤토리에 남은 총알수 >= 충전해야할 총알 수
	if (LeftAmmo >= CountToAddAmmo)
	{
		// by정훈, 추가 충전한 총알 개수만큼 인벤토리에 남은 총알 개수를 줄입니다.
		LeftAmmo -= CountToAddAmmo;
	}
	// by정훈, 인벤토리에 남은 총알수 < 충전해야할 총알 수
	else if (LeftAmmo < CountToAddAmmo)
	{
		// by정훈, 충전할 총알 개수는 인벤토리에 남은 총알 개수로 저장합니다.
		CountToAddAmmo = LeftAmmo;
		// by정훈, 인벤토리에 남은 총알개수는 0개가 됩니다.
		LeftAmmo = 0;
	}
	// by정훈, 탄창에 추가할 수 있는 만큼의 총알을 충전합니다.
	LoadedAmmo += CountToAddAmmo;
	// by정훈, 총알정보UI를 업데이트 합니다.
	CurrentInGameManager->UpdatePlayerGunAmmoUI();
	// by정훈, 리로드 애니메이션을 실행합니다.
	Anim_ReloadOn();

	if (Sfx_Pistol_Reload != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sfx_Pistol_Reload, this->GetActorLocation());
	}

}

void AProjectMSCharacter::MatchQuestItem()
{
	if (FocusedItem->GetItemNumber() == 0)
	{
		CurrentInGameManager->AddQuestObjectiveCollectCountUp();
		CurrentInGameManager->CalculateQuestCompletePercent();
	}
}

// by정훈, 인벤토리에 같은 종류의 아이템이 있는지 체크함
bool AProjectMSCharacter::MatchSameItem()
{
	// 획득한 아이템과 같은 종류의 아이템이 인벤토리에 없는 경우
	if (CurrentInGameManager->CheckSameItemExist(
		FocusedItem->GetItemNumber()) == false)
	{
		return false;
	}
	// 획득한 아이템과 같은 종류의 아이템이 인벤토리에 있는 경우
	else 
	{
		return true;
	}
}

void AProjectMSCharacter::FindCurrentInGameManager()
{
	if (FindActorByName("BP_InGameManager") != nullptr)
	{
		CurrentInGameManager = Cast<AInGameManager>(FindActorByName("BP_InGameManager"));
		UE_LOG(LogTemp, Warning, TEXT("InGameManager founded.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameManager not founded.."));
	}
}

AInGameManager* AProjectMSCharacter::GetCurrentIngameManager()
{
	return CurrentInGameManager;
}

void AProjectMSCharacter::SetResultModeControl()
{
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	GetWorld()->GetFirstPlayerController()->SetInputMode(InputModeData);
}

void AProjectMSCharacter::SetGamePlayModeControl()
{
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
}

// by정훈, 캐릭터가 피해를 입게 됩니다.
void AProjectMSCharacter::GetDamage(float Damage)
{
	// by정훈, 전달된 인자의 값 만큼 현재의 체력이 줄어듭니다.
	CurrentHealth = CurrentHealth - Damage;
	UE_LOG(LogTemp, Warning, TEXT("Current PC health:%f"), CurrentHealth);
	// by정훈, 플레이어 HP가 0 이상인 경우
	if (CurrentHealth > 0)
	{
		// by정훈, 피격 애니메이션을 재생시킵니다.
		ShouldHit = true;
	}
	// by정훈, 플레이어 HP가 0 보다 작거나 같은 경우
	else
	{
		// by정훈, 죽음 애니메이션을 On 합니다.
		DiePlayerCharacter();
		// by정훈, 게임오버UI를 호출합니다.
		CurrentInGameManager->ShowPanel_GameOver();
		// by정훈, 마우스 커서를 보여줍니다.
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		// by정훈, 인풋모드 설정을 마우스 커서를 뷰포트에 고정되도록 합니다.
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		// by정훈, 수정된 인풋모드를 플레이어컨트롤러에 저장합니다.
		GetWorld()->GetFirstPlayerController()->SetInputMode(InputModeData);
	}
}

// by정훈, 피격 애니메이션 시퀀스의 종료 시점 Notify 호출시 호출됩니다.
void AProjectMSCharacter::SwitchShouldHitFalse()
{
	// by정훈, 피격 애니메이션이 더이상 반복하지 않도록 끕니다.
	ShouldHit = false;
}

float AProjectMSCharacter::GetCurrentHP()
{
	return CurrentHealth;
}

void AProjectMSCharacter::SetCurrentHP(float AddHP)
{
	CurrentHealth = CurrentHealth + AddHP;
}

void AProjectMSCharacter::AddCurrentHP(float AddHP)
{
	CurrentHealth = CurrentHealth + AddHP;
}

// by정훈, 남은 탄약의 수를 늘려주는 함수입니다.
void AProjectMSCharacter::AddLeftAmmo(int32 AmmoCount)
{
	// 피스톨 잔탄수량을 AmmoCount만큼 늘려주어 저장합니다.
	LeftAmmo += AmmoCount;

	// 탄약 증가 확인용 로그입니다.
	UE_LOG(LogTemp, Display, TEXT("Current pistol ammo count:%d"), GetLeftAmmo());
}

