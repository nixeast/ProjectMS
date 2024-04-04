// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class PROJECTMS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UFUNCTION(BlueprintCallable)
	void ResetSense(UActorComponent* Component);

	void BindAIController();

	// by정훈, 인스턴스에 생성할 센서 컴포넌트를 가리킬 포인터입니다.
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UPawnSensingComponent* MyPawnSensComp;

	UPROPERTY(EditAnywhere, Category = "AI")
	class UNiagaraSystem* BloodEffect;

	// by정훈, AI컨트롤러가 빙의된 경우 인스턴스 주소를 담을 포인터입니다.
	class AAIController* CurrentAIController;

	// by정훈, Enemy 시야 센서 감지 시 대상 Pawn의 정보를 인지합니다.
	UFUNCTION(BlueprintCallable)
	void TrackTarget(APawn* TargetPawn);

	// by정훈, 감지된 대상을 향해서 이동시킵니다.
	void MoveToTarget(APawn* TargetPawn);

	// by정훈, 추적 대상의 감지 유무를 저장하는 변수입니다.
	bool TrackTargetMode;

	// by정훈, 감지된 대상이 있을경우 대상 Pawn의 주소를 가리킬 포인터입니다.
	class APawn* TrackTargetPawn;

	// by정훈, 근접 공격 애니메이션을 실행시킵니다.
	void Anim_ZombieAttack();

	// by정훈, 근접 공격 범위를 저장합니다.
	UPROPERTY(EditAnywhere, Category = "AI")
	float ZombieAttackRange;

	// by정훈, 추격 인식 범위를 저장합니다.
	UPROPERTY(EditAnywhere, Category = "AI")
	float EnemyChaseRange;

	float DistanceToPlayerPawn;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UCapsuleComponent* AttackArea_01;

	UFUNCTION()
	void Attack_01_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Attack_01_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly)
	float MyDamage;

	void PlayAnim_Reaction_Hit();
	void StopAnim_Reaction_Hit();

	FTimerHandle TimerHandle;

	void DieEnemy();
	void DestroyEnemyInstance();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool ShouldZombieAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool ShouldReaction_Hit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool ShouldDie;

	UFUNCTION(BlueprintCallable)
	void AnimNotifyFunc_TurnOnAtkOverlapEvnet();

	UFUNCTION(BlueprintCallable)
	void AnimNotifyFunc_TurnOffAtkOverlapEvnet();

	UFUNCTION(BlueprintCallable)
	void AnimNotifyFunc_TurnOff_HitReaction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHP;

	void GetDamage(float Damage);
};
