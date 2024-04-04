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

	// by����, �ѱ� �޽ø� �Ҵ� �� ����ƽ�޽� ������Ʈ�� GunMesh�� �����մϴ�.
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	// by����, GunMesh�� ĳ���� �޽��� ������ ���Ͽ� ���������ݴϴ�.
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

// by����, ���� �� ĳ���͸� ȸ����Ű�� �Լ�
void AProjectMSCharacter::SetCharacterOrientationToAim(const FVector& AimTarget)
{
	// ���ش��� ĳ���Ϳ��� ���Ͱ��� ���Ͽ� �������� ��ȯ��ŵ�ϴ�.
	ToAimTarget = AimTarget - GetMesh()->GetComponentLocation();
	ToAimTargetRotation = ToAimTarget.Rotation();
	// ���Ⱚ�� �߰����� �������� ���Ͽ� ĳ������ RootComponent ���Ⱚ�� �����մϴ�.
	LookAtRotation = FRotator(0, ToAimTargetRotation.Yaw + AdditionalAimYawBalance, 0);
	GetCapsuleComponent()->SetWorldRotation(LookAtRotation);
	
}

// by����, �ѱ� �߻� �� ȣ��Ǵ� �Լ�
void AProjectMSCharacter::GunShoot()
{
	// 0=pistol, 1=shotgun
	if (CurrentWeaponNum == 0)
	{
		// by����, ���� ���¿� ������ �Ѿ��� 0 �̻��� ��� �߻�
		if (Aimed == true && LoadedAmmo > 0)
		{
			// by����, ����Ʈ���̽� ����Ͽ� �浹üũ
			TraceGunShootTarget();
			// by����, ������ �Ѿ� ���� ���ҽ�Ŵ
			ConsumeAmmo();
			// by����, �������� �Ѿ�����UI ������Ʈ
			UpdateAmmoUI();
			// by����, ������ �Ѿ��� 0�ϰ�� �Ѿ˾����� �κ��丮���� ����
			if (LeftAmmo + LoadedAmmo <= 0)
			{
				CurrentInGameManager->DeleteInventoryItemButton();
			}
		}

	}
	else
	{
		// by����, ���� ���¿� ������ �Ѿ��� 0 �̻��� ��� �߻�
		if (Aimed == true && LoadedAmmo_Shotgun > 0)
		{
			// by����, ����Ʈ���̽� ����Ͽ� �浹üũ
			TraceGunShootTarget();
			// by����, ������ �Ѿ� ���� ���ҽ�Ŵ
			ConsumeAmmo();
			// by����, �������� �Ѿ�����UI ������Ʈ
			UpdateAmmoUI();
			// by����, ������ �Ѿ��� 0�ϰ�� �Ѿ˾����� �κ��丮���� ����
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
	// by����, �÷��̾� ī�޶� ��ġ�� �ٶ󺸴� ���Ⱚ�� �����ɴϴ�.
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerCamLocation, PlayerCamRotation);
	// by����, ���⸦ ����ִ� ������ ��ġ���� �̿��Ͽ� �߻�� �Ѿ��� ���������� �����մϴ�.
	GunShootTraceStart = this->GetMesh()->GetSocketLocation(TEXT("hand_rSocket")) 
		+ (this->GetMesh()->GetForwardVector().Rotation().Add(0,90.0,0).Vector() * GunFireStartDist);
	// by����, �÷��̾� ī�޶� ���� �̿��Ͽ� �浹üũ ���� ������ ���� �����մϴ�.
	GunShootTraceEnd = PlayerCamLocation + PlayerCamRotation.Vector() * 10000.0f;
	
	if (CurrentWeaponNum == 0)
	{
		// by����, ����Ʈ���̽� �̿��Ͽ� �浹üũ ������� �����մϴ�.
		bSuccess_GunShoot = GetWorld()->LineTraceSingleByChannel(GunShootHit, GunShootTraceStart, AimTraceImpactPoint, ECollisionChannel::ECC_GameTraceChannel2);
		// by����, ����Ʈ���̽��� ������ ������ �ð� üũ�� ������ �����մϴ�.
		DrawDebugLine(GetWorld(), GunShootTraceStart, AimTraceImpactPoint, FColor::Red, false, 2.0f);
		// by����, �Ѿ� �߻� ȿ������ ����մϴ�.
		PlayGunShootSFX();
	}
	else
	{
		// ���� �Ѿ��� �л갪�� ������ �����Դϴ�.
		float DispersePoint = 0;
		// ���� �Ѿ��� �л� ��ġ���� ������ �����Դϴ�.
		FVector DisperseVec = FVector(0, 0, 0);
		// ���� �Ѿ��� ���������� ������ �����Դϴ�.
		FVector NewImpactPoint = AimTraceImpactPoint;

		// ���� �߻� �� �߻��� �Ѿ��� ���� 4���� �����ݴϴ�.
		for (int32 i = 0; i < 4; i++)
		{
			// ����Ʈ���̽� �Ͽ� �߻��� �Ѿ��� ���� ������ üũ�մϴ�.
			bSuccess_GunShoot = GetWorld()->LineTraceSingleByChannel
			(GunShootHit, GunShootTraceStart, NewImpactPoint, ECollisionChannel::ECC_GameTraceChannel2);
			// �Ѿ� �߻��� ������ üũ�ϱ� ���� �뵵�� ����� �Լ��Դϴ�.
			DrawDebugLine(GetWorld(), GunShootTraceStart, NewImpactPoint, FColor::Red, false, 2.0f);
			// ���� �Ѿ��� �л��� ���������� �����մϴ�.
			DispersePoint = (float)FMath::RandRange(ShotgunAngle_Min, ShotgunAngle_Max);
			// ���������� ���� �л갪�� DisperseVec�� X,Y,Z�� �����մϴ�.
			DisperseVec.X = DispersePoint;
			DisperseVec.Y = DispersePoint;
			DisperseVec.Z = DispersePoint;
			// �л갪�� �Էµ� TempVec�� ������ ���� ���� AimTraceImpactPoint�� ���մϴ�.
			NewImpactPoint = AimTraceImpactPoint + DisperseVec;
		}

		PlayGunShootSFX();
	}

	// by����, �浹üũ �Ǿ��� ��� �����մϴ�.
	if (bSuccess_GunShoot == true)
	{
		// by����, ��ƼŬ����Ʈ�� nullptr�� �ƴ��� üũ�մϴ�.
		if (MuzzleParticle != nullptr)
		{
			// by����, ĳ���� �޽��� �����ռ��� ��ġ�� �������� ��ƼŬ�� �����մϴ�.
			UGameplayStatics::SpawnEmitterAttached(
				MuzzleParticle, this->GetMesh(), TEXT("hand_rSocket"),
				FVector(MuzzleSparkStartDist,0,10.0), FRotator(90.0, 0, 0), 
				EAttachLocation::KeepRelativeOffset, true);
		}
		// by����, �浹üũ �� ����� �������� üũ�մϴ�.
		if (GunShootHit.GetActor() == nullptr)
		{
			return;
		}
		else
		{
			// by����, �浹üũ �� ������ �̸��� ����Ͽ� ����׿� �����մϴ�.
			UE_LOG(LogTemp, Display, TEXT("Hit:%s"), *GunShootHit.GetActor()->GetName());
			
			// by����, �浹üũ �� ���Ͱ� Enemy���� üũ�մϴ�.
			if (Cast<AEnemy>(GunShootHit.GetActor()) != nullptr)
			{
				// by����, ����� Enemy������ ��� �������� �����մϴ�.
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
	// by����, ������ ȹ�� ���� �� ��Ŀ���� ������ ������ Ȯ���մϴ�.
	if (FocusedItem == nullptr)
	{
		// by����, ��Ŀ���� �������� ���ٸ� Ȯ�ο� �ڵ带 ����ϰ� �ռ��� �����մϴ�.
		UE_LOG(LogTemp, Display, TEXT("No Focused Item.."));
	}
	// by ����, ��Ŀ�� �� �������� ���� ��� ȹ�� ������ �����մϴ�.
	else
	{
		// by ����, ���� ������ ȹ�� ������� 100% �̻��� �Ǵ� ����� �����Դϴ�.
		if (CurrentHarvestPercent >= 10.0f)
		{
			// by ����, ���� ȹ���� �������� ������ȣ�� �迭�� ��� �����ϵ��� �մϴ�.
			AddMyItemArray(FocusedItem->GetItemNumber());
			// by ����, GameManager�� ���� ȹ���� �������� ��ȣ�� ����ϵ��� �� �ݴϴ�.
			CurrentInGameManager->SetFocusedItemNumber(FocusedItem->GetItemNumber());
			// by ����, ȹ���� ������ ǥ�� UI�� ȣ���մϴ�.
			CurrentInGameManager->ShowUI_ItemGet();
			// by ����, ȹ���� ������ �̹����� ���Կ� ����մϴ�.
			CurrentInGameManager->SetImage_GetItemInfo();
			// by ����, ȹ���� ������ ���� ȭ���� ȣ���մϴ�.
			CurrentInGameManager->ShowUI_GetItemInfo();
			SetResultModeControl();
			// by ����, ���� ������ �������� ����Ʈ ���������� �Ǵ��ϴ� �Լ��� ȣ���մϴ�.
			MatchQuestItem();
			// by ����, ���� ������ �������� ������ ȹ���ߴ� ���������� �Ǵ��ϴ� �Լ��� ȣ���մϴ�.
			bool isMatched = MatchSameItem();
			
			if (isMatched == true)
			{
				// ���� ������ ���� �߰�
				CurrentInGameManager->AddExistItemCount();
			}
			else if(isMatched == false)
			{
				// ������ ��ư �ű� ����
				CurrentInGameManager->CreateItemButton(FocusedItem->GetItemNumber());
				// �κ��丮 �ȿ� ������ ��ư ��ġ
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
			// by ����, ���� ����Ʈ�� �Ϸ��Ͽ����� �Ǵ��ϴ� �Լ��� ȣ���մϴ�.
			CurrentInGameManager->CheckQuestComplete();
			// by ����, ��Ȯ �Ϸ��� ������ �ν��Ͻ��� �����ϰ�, FocusedItem ������ nullptr�� �����Ͽ� �ʱ�ȭ �� �ݴϴ�.
			FocusedItem->DestroyThisItem();
			FocusedItem = nullptr;
			// by ����, ������ ��Ȯ ���¸� �����ϴ� �Լ��� ȣ���մϴ�.
			HarvestOff();
			// by ����, ������ ȹ�� ���¸� UI�� �����Ͽ� �����ݴϴ�.
			ShowMyItemArray();
		}
		// by ����, ������ ȹ�� ���°� 100% �̸��� ����� �����Դϴ�.
		else
		{
			// by����, ������ �����Ӵ� �ð��������� �����Ͽ� ȹ��������� ���������ݴϴ�.
			CurrentHarvestPercent += GetWorld()->DeltaTimeSeconds * 1.0f * 2.0f;
			// by����, �������� ȹ�� ����� ���� ������ ��ȮUI�� ǥ���Ͽ��ݴϴ�.
			// (�����Ӵ� �ð���) * (ProgressBar������) * (ȹ��ӵ��������)
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
			// �ΰ��� UI�� �մϴ�.
			CurrentInGameManager->ShowInGameUI();
			// ���콺Ŀ���� ���̵��� �մϴ�.
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			// InputModeData�� ���콺�� ����Ʈ�� ���̵��� ��޴ϴ�.
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			// ������ InputModeData�� �÷��̾��� ��ǲ��忡 �������ݴϴ�.
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

// by����, ���� �����ϴ� ���⸦ ��ü�մϴ�.
void AProjectMSCharacter::ChangeCurrentWeapon(int32 weaponNum)
{
	// ���� ������ ������ �ٲ� ������ ������ �����մϴ�.
	CurrentWeaponNum = weaponNum;
	
	// ��ü�� ���Ⱑ ���� �� ���
	if (CurrentWeaponNum == 0)
	{
		// ���� ����ƽ �޽÷� �����ϰ� ��ġ���� ������ �ݴϴ�.
		GunMesh->SetStaticMesh(GunMesh_Shotgun);
		GunMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		GunMesh->SetRelativeLocation(FVector(-10.0f, 3.0f, 1.0f));
		// ������ �Ѿ� �߻� ���������� ������ �ݴϴ�.
		GunFireStartDist = 50.0f;
	}
	// ��ü�� ���Ⱑ ������ ���
	else if (CurrentWeaponNum == 1)
	{
		// ���� ����ƽ �޽÷� �����ϰ� ��ġ���� ������ �ݴϴ�.
		GunMesh->SetStaticMesh(GunMesh_Shotgun);
		GunMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		GunMesh->SetRelativeLocation(FVector(-25.0f, 3.0f, 1.0f));
		// ������ �߻� ���������� ������ �ݴϴ�.
		GunFireStartDist = 100.0f;
		// ���ǿ� ������ ź���� �÷��ݴϴ�.
		LoadedAmmo_Shotgun = 10;
		// �������� ������ ź���� �÷��ݴϴ�.
		Ammo_Shotgun = Ammo_Shotgun + 10;
	}
}

void AProjectMSCharacter::DismissDialogu()
{
	//CurrentInGameManager->HideUI_Interaction();
	
	// �ΰ��� UI�� ���ϴ�.
	CurrentInGameManager->HideInGameUI();
	
	// ���콺Ŀ���� ������ �ʵ��� �մϴ�.
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	// InputModeData�� ���콺�� ����Ʈ�� ���̵��� ����� �ʽ��ϴ�.
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// ������ InputModeData�� �÷��̾��� ��ǲ��忡 �������ݴϴ�.
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
	// by����, ������ ��������� ���
	if (HarvestMode == true)
	{
		// by����, ���� �ִϸ��̼� Ʈ���Ÿ� on �մϴ�.
		ShouldKneeling = true;
		// by����, ĳ������ �̵� �Է��� off �մϴ�.
		GetCharacterMovement()->DisableMovement();
	}
	// by����, ������ ������尡 �ƴ� ���
	else if(HarvestMode == false)
	{
		//by ����, ���� �ִϸ��̼� Ʈ���Ÿ� off �մϴ�.
		ShouldKneeling = false;
		// by����, ĳ������ �̵� �Է��� on �մϴ�.
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
	// by����, �÷��̾�ĳ���� ���� �ִϸ��̼� ����.
	ShouldDie = true;
}

void AProjectMSCharacter::Anim_ReloadOn()
{
	// by����, �ִϸ��̼� �������Ʈ���� ���ε� �ִϸ��̼��� ����ǵ��� �մϴ�.
	ShouldReload = true;
}



void AProjectMSCharacter::Anim_ReloadOff()
{
	// by����, �ִϸ��̼� �������Ʈ���� ���ε� ���°� ����ǵ��� �մϴ�.
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

// by����, �κ��丮 ����Ű�� ���� UI�� ȣ���մϴ�.
void AProjectMSCharacter::InventorySwitch()
{
	// �κ��丮�� �������� ���.
	if (InventoryMode == false)
	{
		// �κ��丮��� UI�� ��ȯ�մϴ�.
		CurrentInGameManager->ShowUI_InventoryMode();
		// �ΰ��� UI�� �մϴ�.
		CurrentInGameManager->ShowInGameUI();
		// �κ��丮 ��带 on �մϴ�.
		InventoryMode = true;
		// ���콺Ŀ���� ���̵��� �մϴ�.
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		// InputModeData�� ���콺�� ����Ʈ�� ���̵��� ��޴ϴ�.
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		// ������ InputModeData�� �÷��̾��� ��ǲ��忡 �������ݴϴ�.
		GetWorld()->GetFirstPlayerController()->SetInputMode(InputModeData);
	}
	// by����, �κ��丮�� �������� ���.
	else
	{
		// �ΰ��� UI�� ���ϴ�.
		CurrentInGameManager->HideInGameUI();
		// �κ��丮��带 off�մϴ�.
		InventoryMode = false;
		// ���콺Ŀ���� ������ �ʵ��� �մϴ�.
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
		// InputModeData�� ���콺�� ����Ʈ�� ���̵��� ����� �ʽ��ϴ�.
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		// ������ InputModeData�� �÷��̾��� ��ǲ��忡 �������ݴϴ�.
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
	}
}

int32 AProjectMSCharacter::GetMagazineCapacity()
{
	return MagazineCapacity;
}

// by����, �������� ������ źâ�� �������մϴ�.
void AProjectMSCharacter::ReloadCurrentWeaponAmmo()
{
	int32 CountToAddAmmo = 0;
	CountToAddAmmo = MagazineCapacity - LoadedAmmo;
	// by����, �κ��丮�� ���� �Ѿ˼� >= �����ؾ��� �Ѿ� ��
	if (LeftAmmo >= CountToAddAmmo)
	{
		// by����, �߰� ������ �Ѿ� ������ŭ �κ��丮�� ���� �Ѿ� ������ ���Դϴ�.
		LeftAmmo -= CountToAddAmmo;
	}
	// by����, �κ��丮�� ���� �Ѿ˼� < �����ؾ��� �Ѿ� ��
	else if (LeftAmmo < CountToAddAmmo)
	{
		// by����, ������ �Ѿ� ������ �κ��丮�� ���� �Ѿ� ������ �����մϴ�.
		CountToAddAmmo = LeftAmmo;
		// by����, �κ��丮�� ���� �Ѿ˰����� 0���� �˴ϴ�.
		LeftAmmo = 0;
	}
	// by����, źâ�� �߰��� �� �ִ� ��ŭ�� �Ѿ��� �����մϴ�.
	LoadedAmmo += CountToAddAmmo;
	// by����, �Ѿ�����UI�� ������Ʈ �մϴ�.
	CurrentInGameManager->UpdatePlayerGunAmmoUI();
	// by����, ���ε� �ִϸ��̼��� �����մϴ�.
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

// by����, �κ��丮�� ���� ������ �������� �ִ��� üũ��
bool AProjectMSCharacter::MatchSameItem()
{
	// ȹ���� �����۰� ���� ������ �������� �κ��丮�� ���� ���
	if (CurrentInGameManager->CheckSameItemExist(
		FocusedItem->GetItemNumber()) == false)
	{
		return false;
	}
	// ȹ���� �����۰� ���� ������ �������� �κ��丮�� �ִ� ���
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

// by����, ĳ���Ͱ� ���ظ� �԰� �˴ϴ�.
void AProjectMSCharacter::GetDamage(float Damage)
{
	// by����, ���޵� ������ �� ��ŭ ������ ü���� �پ��ϴ�.
	CurrentHealth = CurrentHealth - Damage;
	UE_LOG(LogTemp, Warning, TEXT("Current PC health:%f"), CurrentHealth);
	// by����, �÷��̾� HP�� 0 �̻��� ���
	if (CurrentHealth > 0)
	{
		// by����, �ǰ� �ִϸ��̼��� �����ŵ�ϴ�.
		ShouldHit = true;
	}
	// by����, �÷��̾� HP�� 0 ���� �۰ų� ���� ���
	else
	{
		// by����, ���� �ִϸ��̼��� On �մϴ�.
		DiePlayerCharacter();
		// by����, ���ӿ���UI�� ȣ���մϴ�.
		CurrentInGameManager->ShowPanel_GameOver();
		// by����, ���콺 Ŀ���� �����ݴϴ�.
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		// by����, ��ǲ��� ������ ���콺 Ŀ���� ����Ʈ�� �����ǵ��� �մϴ�.
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		// by����, ������ ��ǲ��带 �÷��̾���Ʈ�ѷ��� �����մϴ�.
		GetWorld()->GetFirstPlayerController()->SetInputMode(InputModeData);
	}
}

// by����, �ǰ� �ִϸ��̼� �������� ���� ���� Notify ȣ��� ȣ��˴ϴ�.
void AProjectMSCharacter::SwitchShouldHitFalse()
{
	// by����, �ǰ� �ִϸ��̼��� ���̻� �ݺ����� �ʵ��� ���ϴ�.
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

// by����, ���� ź���� ���� �÷��ִ� �Լ��Դϴ�.
void AProjectMSCharacter::AddLeftAmmo(int32 AmmoCount)
{
	// �ǽ��� ��ź������ AmmoCount��ŭ �÷��־� �����մϴ�.
	LeftAmmo += AmmoCount;

	// ź�� ���� Ȯ�ο� �α��Դϴ�.
	UE_LOG(LogTemp, Display, TEXT("Current pistol ammo count:%d"), GetLeftAmmo());
}

