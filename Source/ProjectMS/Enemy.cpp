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

	// by정훈, Pawn을 감지할 수 있는 센서를 생성합니다.
	MyPawnSensComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("MyPawnSensComp"));

	// by정훈, 근접 공격 범위 초기값을 설정해줍니다.
	ZombieAttackRange = 100.0f;
	
	// by정훈, 추격 가능 범위 초기값을 설정해줍니다.
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
	// by정훈, 감지된 대상이 nullptr이 아니고 플레이어캐릭터의 폰인지 체크합니다.
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

		// by정훈, 추적 모드를 on으로 스위치합니다.
		TrackTargetMode = true;
		// by정훈, 센서에 의해 감지된 Pawn을 포인터변수에 저장합니다.
		TrackTargetPawn = TargetPawn;
		// by정훈, 현재 enemy와 대상 간의 거리를 확인하여 저장합니다.
		DistanceToPlayerPawn = GetDistanceTo(Cast<AActor>(TrackTargetPawn));
		//UE_LOG(LogTemp, Display, TEXT("Distance to target:%f"), DistanceToPlayerPawn);
		
		// by정훈, 대상이 근접공격 범위 안에 위치할 경우
		if (DistanceToPlayerPawn < ZombieAttackRange)
		{
			//by정훈, 이동을 멈춥니다.
			GetCharacterMovement()->StopActiveMovement();
			CurrentAIController->StopMovement();
			//by정훈, 근접공격을 실행합니다.
			Anim_ZombieAttack();
		}
		// by정훈, 대상이 추격 범위 안에 위치할 경우
		else if (DistanceToPlayerPawn < EnemyChaseRange)
		{
			// by정훈, 현재 공격 중이 아닐 경우 체크
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
				//by정훈, 추격을 시작합니다.
				MoveToTarget(TrackTargetPawn);
			}
		}
		// by정훈, 대상이 추격 범위 밖에 위치할 경우
		else
		{
			//by정훈, 이동을 멈춥니다.
			GetCharacterMovement()->StopActiveMovement();
			//by정훈, 근접 공격을 멈춥니다.
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

// by정훈, 공격 애니메이션의 AtkOverlapOn 노티파이 이벤트 호출시 동작
void AEnemy::AnimNotifyFunc_TurnOnAtkOverlapEvnet()
{
	// by정훈, 적의 공격부위 충돌체크가 off상태인지 체크
	if (AttackArea_01->GetGenerateOverlapEvents() == false)
	{
		// by정훈, 공격부위 충돌체크 기능을 켬
		AttackArea_01->SetGenerateOverlapEvents(true);
		UE_LOG(LogTemp, Display, TEXT("SetGenerateOverlapEvents(true)"));
	}
}

// by정훈, 공격 애니메이션의 AtkOverlapOff 노티파이 이벤트 호출시 동작
void AEnemy::AnimNotifyFunc_TurnOffAtkOverlapEvnet()
{
	// by정훈, 적의 공격부위 충돌체크가 on상태인지 체크
	if (AttackArea_01->GetGenerateOverlapEvents() == true)
	{
		// by정훈, 공격부위 충돌체크 기능을 끔
		AttackArea_01->SetGenerateOverlapEvents(false);
		UE_LOG(LogTemp, Display, TEXT("SetGenerateOverlapEvents(false)"));
		// by정훈, 공격 애니메이션 반복되지 않도록 Bind 된 Bool변수를 off 함.
		ShouldZombieAttack = false;
	}
}

void AEnemy::AnimNotifyFunc_TurnOff_HitReaction()
{
	StopAnim_Reaction_Hit();
}

void AEnemy::GetDamage(float Damage)
{
	// by정훈, 인자로 전달된 Damage값 만큼 체력이 감소함
	CurrentHP = CurrentHP - Damage;
	// by정훈, 좀비의 체력이 0 이하일 경우 좀비가 죽습니다.
	if (CurrentHP <= 0)
	{
		MyPawnSensComp->bSeePawns = false;
		CurrentAIController->StopMovement();
		// by정훈, 피격 시 출혈 이펙트를 루트컴포넌트에 생성시킵니다.
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached
		(BloodEffect, GetRootComponent(), NAME_None, FVector(0.f), FRotator(0.f), 
			EAttachLocation::Type::KeepRelativeOffset, true);
		// by정훈, 죽음 애니메이션을 실행시킵니다.
		DieEnemy();
	}
	else
	{
		CurrentAIController->StopMovement();
		// by정훈, 피격 애니메이션을 실행시킵니다.
		PlayAnim_Reaction_Hit();
		// by정훈, 피격 시 출혈 이펙트를 루트컴포넌트에 생성시킵니다.
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached
		(BloodEffect, GetRootComponent(), NAME_None, FVector(0.f), FRotator(0.f), 
			EAttachLocation::Type::KeepRelativeOffset, true);
		UE_LOG(LogTemp, Display, TEXT("CurrentHP:%f"), CurrentHP);
	}
}

// by정훈, 적의 공격부위와 캐릭터의 오버랩 시작 시 호출 되는 함수
void AEnemy::Attack_01_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// by정훈, 충돌한 대상이 플레이어 캐릭터인지 체크
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Attack_01_Start.."));
		// by정훈, 플레이어 캐릭터에게 현재 적의 데미지를 전달하여 피해를 줌
		Cast<AProjectMSCharacter>(OtherActor)->GetDamage(MyDamage);
	}
}

// by정훈, 적의 공격부위와 캐릭터의 오버랩 종료 시 호출 되는 함수
void AEnemy::Attack_01_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// by정훈, 충돌한 대상이 플레이어 캐릭터인지 체크
	if (Cast<AProjectMSCharacter>(OtherActor) != nullptr)
	{
		// by정훈, 오버랩 종료를 체크하기위한 로그 호출.
		UE_LOG(LogTemp, Display, TEXT("Attack_01_End.."));
	}
}

void AEnemy::PlayAnim_Reaction_Hit()
{
	// by정훈, 현재 피격 애니메이션 재생 중일 경우
	if (ShouldReaction_Hit == true)
	{
		// by정훈, 피격 애니메이션 시작 스위치를 끔
		ShouldReaction_Hit = false;
		// by정훈, 0.1초 후 피격 애니메이션 다시 시작
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle, this, &AEnemy::PlayAnim_Reaction_Hit, 0.1f, false
		);
	}
	// by정훈, 현재 피격 애니메이션 재생이 아닐 경우
	else{ShouldReaction_Hit = true;}
}

void AEnemy::StopAnim_Reaction_Hit()
{
	ShouldReaction_Hit = false;
}

void AEnemy::DieEnemy()
{
	// by정훈, 좀비가 죽는 애니메이션을 실행
	ShouldDie = true;
	// by정훈, 3초 후 좀비 액터 제거 함수를 호출합니다.
	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle, this, &AEnemy::DestroyEnemyInstance, 3.0f, false
	);
}

void AEnemy::DestroyEnemyInstance()
{
	// by정훈, 해당하는 좀비 액터를 제거합니다.
	this->Destroy();
}
