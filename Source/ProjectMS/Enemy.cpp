// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectMSCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// by����, Pawn�� ������ �� �ִ� ������ �����մϴ�.
	MyPawnSensComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("MyPawnSensComp"));

	// by����, ���� ���� ���� �ʱⰪ�� �������ݴϴ�.
	ZombieAttackRange = 100.0f;
	
	// by����, �߰� ���� ���� �ʱⰪ�� �������ݴϴ�.
	EnemyChaseRange = 1200.0f;

	AttackArea_01 = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackArea"));
	AttackArea_01->SetupAttachment(GetMesh(), TEXT("RightHandSocket"));
	//AttackArea->SetupAttachment(GetMesh());
	AttackArea_01->SetCapsuleHalfHeight(11.0f);
	AttackArea_01->SetCapsuleRadius(11.0f);

	AttackArea_01->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::Attack_01_BeginOverlap);
	AttackArea_01->OnComponentEndOverlap.AddDynamic(this, &AEnemy::Attack_01_EndOverlap);

	MyDamage = 25.0f;
	MaxHP = 100.0f;
	CurrentHP = MaxHP;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	MyPawnSensComp->OnSeePawn.AddDynamic(this, &AEnemy::TrackTarget);

	MyPawnSensComp->OnComponentDeactivated.AddDynamic(this, &AEnemy::ResetSense);

	BindAIController();

	//AttackArea->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("RightHandSocket"));
}

// Called every frame
//void AEnemy::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//	
//}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::TrackTarget(APawn* TargetPawn)
{
	// by����, ������ ����� nullptr�� �ƴϰ� �÷��̾�ĳ������ ������ üũ�մϴ�.
	if (TargetPawn != nullptr && TargetPawn == GetWorld()->GetFirstPlayerController()->GetPawn())
	{

		if (Cast<AProjectMSCharacter>(TargetPawn)->GetCurrentHP() <= 0)
		{
			TrackTargetMode = false;
			ShouldZombieAttack = false;
			GetCharacterMovement()->StopActiveMovement();
			CurrentAIController->StopMovement();
			return;
		}

		// by����, ���� ��带 on���� ����ġ�մϴ�.
		TrackTargetMode = true;
		// by����, ������ ���� ������ Pawn�� �����ͺ����� �����մϴ�.
		TrackTargetPawn = TargetPawn;
		// by����, ���� enemy�� ��� ���� �Ÿ��� Ȯ���Ͽ� �����մϴ�.
		DistanceToPlayerPawn = GetDistanceTo(Cast<AActor>(TrackTargetPawn));
		//UE_LOG(LogTemp, Display, TEXT("Distance to target:%f"), DistanceToPlayerPawn);
		
		// by����, ����� �������� ���� �ȿ� ��ġ�� ���
		if (DistanceToPlayerPawn < ZombieAttackRange)
		{
			//by����, �̵��� ����ϴ�.
			GetCharacterMovement()->StopActiveMovement();
			CurrentAIController->StopMovement();
			//by����, ���������� �����մϴ�.
			Anim_ZombieAttack();
		}
		// by����, ����� �߰� ���� �ȿ� ��ġ�� ���
		else if (DistanceToPlayerPawn < EnemyChaseRange)
		{
			// by����, ���� ���� ���� �ƴ� ��� üũ
			if(ShouldZombieAttack == true)
			{
				return;
			}
			else if (ShouldReaction_Hit == true)
			{
				return;
			}
			else
			{
				//by����, �߰��� �����մϴ�.
				MoveToTarget(TrackTargetPawn);
			}
		}
		// by����, ����� �߰� ���� �ۿ� ��ġ�� ���
		else
		{
			//by����, �̵��� ����ϴ�.
			GetCharacterMovement()->StopActiveMovement();
			//by����, ���� ������ ����ϴ�.
			ShouldZombieAttack = false;
		}

	}
}

void AEnemy::ResetSense(UActorComponent* Component)
{
	TrackTargetMode = false;
	UE_LOG(LogTemp, Display, TEXT("Deactivate completed.."));
}

void AEnemy::MoveToTarget(APawn* TargetPawn)
{
	if (TargetPawn != nullptr)
	{
		CurrentAIController->MoveToActor(Cast<AActor>(TargetPawn), 50.0f, false);
		//UE_LOG(LogTemp, Warning, TEXT("Move to actor.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find targetPawn failed.."));
	}
}

void AEnemy::BindAIController()
{
	CurrentAIController = Cast<AAIController>(GetController());
}

void AEnemy::Anim_ZombieAttack()
{
	CurrentAIController->StopMovement();
	
	ShouldZombieAttack = true;
}

// by����, ���� �ִϸ��̼��� AtkOverlapOn ��Ƽ���� �̺�Ʈ ȣ��� ����
void AEnemy::AnimNotifyFunc_TurnOnAtkOverlapEvnet()
{
	// by����, ���� ���ݺ��� �浹üũ�� off�������� üũ
	if (AttackArea_01->GetGenerateOverlapEvents() == false)
	{
		// by����, ���ݺ��� �浹üũ ����� ��
		AttackArea_01->SetGenerateOverlapEvents(true);
		UE_LOG(LogTemp, Display, TEXT("SetGenerateOverlapEvents(true)"));
	}
}

// by����, ���� �ִϸ��̼��� AtkOverlapOff ��Ƽ���� �̺�Ʈ ȣ��� ����
void AEnemy::AnimNotifyFunc_TurnOffAtkOverlapEvnet()
{
	// by����, ���� ���ݺ��� �浹üũ�� on�������� üũ
	if (AttackArea_01->GetGenerateOverlapEvents() == true)
	{
		// by����, ���ݺ��� �浹üũ ����� ��
		AttackArea_01->SetGenerateOverlapEvents(false);
		UE_LOG(LogTemp, Display, TEXT("SetGenerateOverlapEvents(false)"));
		// by����, ���� �ִϸ��̼� �ݺ����� �ʵ��� Bind �� Bool������ off ��.
		ShouldZombieAttack = false;
	}
}

void AEnemy::AnimNotifyFunc_TurnOff_HitReaction()
{
	StopAnim_Reaction_Hit();
}

void AEnemy::GetDamage(float Damage)
{
	// by����, ���ڷ� ���޵� Damage�� ��ŭ ü���� ������
	CurrentHP = CurrentHP - Damage;
	// by����, ������ ü���� 0 ������ ��� ���� �׽��ϴ�.
	if (CurrentHP <= 0)
	{
		MyPawnSensComp->bSeePawns = false;
		CurrentAIController->StopMovement();
		// by����, �ǰ� �� ���� ����Ʈ�� ��Ʈ������Ʈ�� ������ŵ�ϴ�.
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached
		(BloodEffect, GetRootComponent(), NAME_None, FVector(0.f), FRotator(0.f), 
			EAttachLocation::Type::KeepRelativeOffset, true);
		// by����, ���� �ִϸ��̼��� �����ŵ�ϴ�.
		DieEnemy();
	}
	else
	{
		CurrentAIController->StopMovement();
		// by����, �ǰ� �ִϸ��̼��� �����ŵ�ϴ�.
		PlayAnim_Reaction_Hit();
		// by����, �ǰ� �� ���� ����Ʈ�� ��Ʈ������Ʈ�� ������ŵ�ϴ�.
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached
		(BloodEffect, GetRootComponent(), NAME_None, FVector(0.f), FRotator(0.f), 
			EAttachLocation::Type::KeepRelativeOffset, true);
		UE_LOG(LogTemp, Display, TEXT("CurrentHP:%f"), CurrentHP);
	}
}

// by����, ���� ���ݺ����� ĳ������ ������ ���� �� ȣ�� �Ǵ� �Լ�
void AEnemy::Attack_01_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// by����, �浹�� ����� �÷��̾� ĳ�������� üũ
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Attack_01_Start.."));
		// by����, �÷��̾� ĳ���Ϳ��� ���� ���� �������� �����Ͽ� ���ظ� ��
		Cast<AProjectMSCharacter>(OtherActor)->GetDamage(MyDamage);
	}
}

// by����, ���� ���ݺ����� ĳ������ ������ ���� �� ȣ�� �Ǵ� �Լ�
void AEnemy::Attack_01_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// by����, �浹�� ����� �÷��̾� ĳ�������� üũ
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		// by����, ������ ���Ḧ üũ�ϱ����� �α� ȣ��.
		UE_LOG(LogTemp, Display, TEXT("Attack_01_End.."));
	}
}

void AEnemy::PlayAnim_Reaction_Hit()
{
	// by����, ���� �ǰ� �ִϸ��̼� ��� ���� ���
	if (ShouldReaction_Hit == true)
	{
		// by����, �ǰ� �ִϸ��̼� ���� ����ġ�� ��
		ShouldReaction_Hit = false;
		// by����, 0.1�� �� �ǰ� �ִϸ��̼� �ٽ� ����
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle, this, &AEnemy::PlayAnim_Reaction_Hit, 0.1f, false
		);
	}
	// by����, ���� �ǰ� �ִϸ��̼� ����� �ƴ� ���
	else{ShouldReaction_Hit = true;}
}

void AEnemy::StopAnim_Reaction_Hit()
{
	ShouldReaction_Hit = false;
}

void AEnemy::DieEnemy()
{
	// by����, ���� �״� �ִϸ��̼��� ����
	ShouldDie = true;
	// by����, 3�� �� ���� ���� ���� �Լ��� ȣ���մϴ�.
	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle, this, &AEnemy::DestroyEnemyInstance, 3.0f, false
	);
}

void AEnemy::DestroyEnemyInstance()
{
	// by����, �ش��ϴ� ���� ���͸� �����մϴ�.
	this->Destroy();
}
