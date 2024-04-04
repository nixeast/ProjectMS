// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ProjectMSCharacter.generated.h"


UCLASS(config=Game)
class AProjectMSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Interaction;

	/** GunShoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* GunShootAction;

	/** Harvest Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* HarvestAction;

	/** Inventory Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InventoryAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

public:
	AProjectMSCharacter();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Aim();
	void Aim_End();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	
	UPROPERTY(EditAnywhere, Category="Aim")
	float CameraboomLength_Aimed;

	UPROPERTY(EditAnywhere, Category = "Aim")
	float CameraboomLength_Origin;

	int32 CurrentMovementMode = 0;

	bool Aimed = false;

	void ChangeMovementMode();

	void UpdateMovementSpeed();

	UPROPERTY(EditAnywhere, Category = "Walk")
	float WalkSpeed_Origin;

	UPROPERTY(EditAnywhere, Category = "Walk")
	float WalkSpeed_Aim;

	void Interact();

	void TraceAimTarget();

	FVector PlayerCamLocation;
	FRotator PlayerCamRotation;
	FVector AimTraceStart;
	FVector AimTraceEnd;
	FHitResult AimHit;
	bool bSuccess_Aim;
	FVector AimTraceImpactPoint;

	UPROPERTY(EditAnywhere, Category = "GameManager")
	class AInGameManager* CurrentInGameManager;

	class AActor* FindActorByName(FString ActorName);

	void SetCharacterOrientationToAim(const FVector& AimTarget);
	FVector ToAimTarget;
	FRotator ToAimTargetRotation;
	FRotator LookAtRotation;

	UPROPERTY(EditAnywhere, Category = "Aim")
	double AdditionalAimYawBalance;

	void GunShoot();
	void TraceGunShootTarget();

	FVector GunShootTraceStart;
	FVector GunShootTraceEnd;
	bool bSuccess_GunShoot;
	FHitResult GunShootHit;

	void ConsumeAmmo();

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	int32 LoadedAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	int32 LoadedAmmo_Shotgun;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	int32 LeftAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	int32 MagazineCapacity;


	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MyDamage;

	void UpdateAmmoUI();

	void Harvest();
	void HarvestOn();
	void HarvestOff();

	

	float CurrentHarvestPercent;
	bool HarvestMode;

	TArray<int32> MyItemArray;

	void ShowMyItemArray();
	void AddMyItemArray(int32 ItemNumber);

	bool InventoryMode;
	

	FInputModeGameAndUI InputModeData;

	void ReloadCurrentWeaponAmmo();
	
	void MatchQuestItem();
	bool MatchSameItem();
	void Anim_Harvest();

	void Anim_AimedOn();
	void Anim_AimedOff();

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* GunMesh;

	

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	class USoundBase* Sfx_Pistol_Shoot;

	void PlayGunShootSFX();

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	class USoundBase* Sfx_Pistol_Reload;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	float GunFireStartDist;

	void DiePlayerCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	UParticleSystem* MuzzleParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	float MuzzleSparkStartDist;

	void Anim_ReloadOn();
	
	void CallDialogue();

	void OpenCurrentGateDoor();

	int32 CurrentWeaponNum;

	void ChangeCurrentWeapon(int32 weaponNum);

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	int32 ShotgunAngle_Min;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	int32 ShotgunAngle_Max;

public:

	void DismissDialogu();

	class AItem* FocusedItem;

	bool IsCompletedQuest;

	void SetFocusedItem(class AItem* TargetItem);

	void ResetFocusedItem();

	int32 GetLoadedAmmo();

	int32 GetLeftAmmo();
	void AddLeftAmmo(int32 AmmoCount);

	int32 GetMagazineCapacity();

	void FindCurrentInGameManager();

	class AInGameManager* GetCurrentIngameManager();

	void SetResultModeControl();
	void SetGamePlayModeControl();

	// by정훈, 아이템 수집액션 시작 및 종료 트리거 변수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harvest")
	bool ShouldKneeling = false;

	void GetDamage(float Damage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool ShouldHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool ShouldAim = false;

	UFUNCTION(BlueprintCallable)
	void SwitchShouldHitFalse();

	UFUNCTION(BlueprintCallable)
	void Anim_ReloadOff();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool ShouldDie = false;

	float GetCurrentHP();
	void SetCurrentHP(float AddHP);
	void AddCurrentHP(float AddHP);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool ShouldReload = false; 
	
	void InventorySwitch();

	UPROPERTY(EditAnywhere)
	bool HasGateKey;

	bool CanActiveInteractionObj;

	class AGate* CurrentGate = nullptr;

	class AItem* CurrentHarvestTarget = nullptr;

	void ActiveCurrentGate();

	UPROPERTY(EditAnywhere)
	bool HasElevatorCardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	class UStaticMesh* GunMesh_Shotgun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	class UStaticMesh* GunMesh_Pistol;

	UPROPERTY(EditAnywhere, Category = "Gun")
	float Ammo_Shotgun;
};

