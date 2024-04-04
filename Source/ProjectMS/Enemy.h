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

	// by����, �ν��Ͻ��� ������ ���� ������Ʈ�� ����ų �������Դϴ�.
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UPawnSensingComponent* MyPawnSensComp;

	UPROPERTY(EditAnywhere, Category = "AI")
	class UNiagaraSystem* BloodEffect;

	// by����, AI��Ʈ�ѷ��� ���ǵ� ��� �ν��Ͻ� �ּҸ� ���� �������Դϴ�.
	class AAIController* CurrentAIController;

	// by����, Enemy �þ� ���� ���� �� ��� Pawn�� ������ �����մϴ�.
	UFUNCTION(BlueprintCallable)
	void TrackTarget(APawn* TargetPawn);

	// by����, ������ ����� ���ؼ� �̵���ŵ�ϴ�.
	void MoveToTarget(APawn* TargetPawn);

	// by����, ���� ����� ���� ������ �����ϴ� �����Դϴ�.
	bool TrackTargetMode;

	// by����, ������ ����� ������� ��� Pawn�� �ּҸ� ����ų �������Դϴ�.
	class APawn* TrackTargetPawn;

	// by����, ���� ���� �ִϸ��̼��� �����ŵ�ϴ�.
	void Anim_ZombieAttack();

	// by����, ���� ���� ������ �����մϴ�.
	UPROPERTY(EditAnywhere, Category = "AI")
	float ZombieAttackRange;

	// by����, �߰� �ν� ������ �����մϴ�.
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
