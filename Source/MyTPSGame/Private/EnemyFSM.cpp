// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "TPSPlayer.h"
#include "../MyTPSGame.h"
#include <Components/CapsuleComponent.h>
#include "EnemyAnim.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "PathManager.h"
#include "MyTPSGame/MyTPSGameGameModeBase.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	state = EEnemyState::IDLE; // 명시적, 암시적
	moveSubState = EEnemyMoveSubState::PATROL;

	// me를 찾고싶다.
	me = Cast<AEnemy>(GetOwner());

	ai = Cast<AAIController>(me->GetController());

	// 태어날 때 현재 체력을 최대 체력으로 하고싶다.
	me->hp = me->maxHP;

	// 태어날 때 랜덤 목적지를 정해놓고싶다.
	UpdateRandomLocation(randLocationRadius, randomLocation);

	// 레벨에 존재하는 PathManager를 찾고싶다.
	pathManager = Cast<APathManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APathManager::StaticClass()));

}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 내 본체의 EnemyAnim의 state에 내 state를 넣어주고싶다.

	//me->enemyAnim->state = this->state;


	switch (state)
	{
	case EEnemyState::IDLE:
		TickIdle();
		break;
	case EEnemyState::MOVE:
		TickMove();
		break;
	case EEnemyState::ATTACK:
		TickAttack();
		break;
	case EEnemyState::DAMAGE:
		TickDamage();
		break;
	case EEnemyState::DIE:
		TickDie();
		break;
	}
}

//  대기, 플레이어를 찾으면 이동으로 전이
void UEnemyFSM::TickIdle()
{
	// 1. 플레이어를 찾고싶다.
	target = Cast<ATPSPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	// 2. 만약 플레이어를 찾았으면
	if (nullptr != target)
	{
		// 3. 이동으로 전이하고싶다.
		SetState(EEnemyState::MOVE);

	}
}

void UEnemyFSM::TickMoveOldMove()
{
	// A. 내가 갈 수 있는 길위에 target이 있는가?
	UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	FPathFindingQuery query;
	FAIMoveRequest requst;
	requst.SetGoalLocation(target->GetActorLocation());
	requst.SetAcceptanceRadius(5);
	ai->BuildPathfindingQuery(requst, query);

	FPathFindingResult result = ns->FindPathSync(query);
	if (result.Result == ENavigationQueryResult::Success)
	{
		// B. 갈 수 있다면 target쪽으로 이동
		ai->MoveToLocation(target->GetActorLocation());
	}
	else
	{
		// C. 그렇지 않다면 무작위로 위치를 하나 선정해서 그곳으로 가고싶다.
		auto r = ai->MoveToLocation(randomLocation);
		if (r == EPathFollowingRequestResult::AlreadyAtGoal
			||
			r == EPathFollowingRequestResult::Failed)
		{
			// D.    만약 위치에 도착했다면 다시 무작위로 위치를 재선정하고싶다.
			UpdateRandomLocation(randLocationRadius, randomLocation);
		}

	}
}

// 순찰할 위치를 순서대로 이동하고싶다.
void UEnemyFSM::TickPatrol()
{
	FVector patrolTarget = pathManager->waypoints[wayIndex]->GetActorLocation();

	auto result = ai->MoveToLocation(patrolTarget);
	// 만약 순찰위치에 도착했다면
	if (result == EPathFollowingRequestResult::AlreadyAtGoal
		||
		result == EPathFollowingRequestResult::Failed)
	{

		//wayIndex = FMath::RandRange(0, pathManager->waypoints.Num() - 1);

		// 순찰할위치를 다음위치로 갱신하고싶다.
		// 순방향
		wayIndex++;
		// wayIndex의 값이 pathManager->waypoints의 크기 이상이면
		if (wayIndex >= pathManager->waypoints.Num())
		{
			// wayIndex의 값을 0으로 하고싶다.
			wayIndex = 0;
		}

		//int arrayLength = pathManager->waypoints.Num();

		// 순방향
		//wayIndex = (wayIndex + 1) % arrayLength;
		// 역방향
		//wayIndex = (wayIndex + arrayLength - 1) % arrayLength;

		// 역방향
		//wayIndex--;
		//if (wayIndex < 0)
		//{
		//	wayIndex = pathManager->waypoints.Num() - 1;
		//}
	}

	// target과의 거리를 측정해서
	float dist = me->GetDistanceTo(target);
	// 만약 타겟이 감지거리(detectDistance) 안에 들어왔다면
	if (dist < detectDistance)
	{
		// Chase상태로 전이하고싶다.
		moveSubState = EEnemyMoveSubState::CHASE;
	}
}

void UEnemyFSM::TickChase()
{
	// 목적지를 향해서 이동하고싶다.
	ai->MoveToLocation(target->GetActorLocation());

	// 1. 목적지를 향하는 방향을 만들고
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	// 3. 목적지와의 거리를 재고
	float dist = dir.Size();
	// 만약  공격가능거리라면
	if (dist <= attakcRange) {
		// 4. 공격상태로 전이하고싶다.
		SetState(EEnemyState::ATTACK);
	}
	else if (dist > abandonDistance)// 그렇지 않고 포기거리보다 멀어졌다면
	{
		moveSubState = EEnemyMoveSubState::PATROL;
	}
}

// 목적지를 향해서 이동하고싶다.
// 목적지와의 거리가 공격가능거리라면
// 공격상태로 전이하고싶다.
void UEnemyFSM::TickMove()
{
	switch (moveSubState)
	{
	case EEnemyMoveSubState::PATROL:
		TickPatrol();
		break;
	case EEnemyMoveSubState::CHASE:
		TickChase();
		break;
	case EEnemyMoveSubState::OLD_MOVE: 
		TickMoveOldMove();
		break;
	}




}

// 공격 타이밍 
void UEnemyFSM::TickAttack()
{
	// 1. 시간이 흐르다가
	currentTime += GetWorld()->GetDeltaSeconds();
	
	// 4. 공격동작이 끝났다면
	if (currentTime > attackDelayTime)
	{
		// 5. 계속 공격을 할 것인지 판단하고싶다.
		// 거리를 구하고
		float dist = target->GetDistanceTo(me);
		// 공격거리보다 멀어졌다면(도망갔다면)
		if (dist > attakcRange)
		{
			// 이동상태로 전이하고싶다.
			SetState(EEnemyState::MOVE);
		}
		else { // 공격거리 안에 있으면 계속해서 공격하고싶다.
			currentTime = 0;
			bAttackPlay = false;
			me->enemyAnim->bAttackPlay = true;
		}
	}
}

// player->enemy 공격
void UEnemyFSM::TickDamage()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	if (currentTime > 1)
	{
		SetState(EEnemyState::MOVE);
		currentTime = 0;
	}
}

void UEnemyFSM::TickDie()
{
	// 만약 넘어지는 애니메이션이 끝나지 않았다면
	if (false == me->enemyAnim->bEnemyDieEnd)
	{
		return;
	}

	currentTime += GetWorld()->GetDeltaSeconds();

	// P = P0 + vt
	FVector p0 = me->GetActorLocation();
	FVector vt = FVector(0, 0, -1) * 200 * GetWorld()->GetDeltaSeconds();

	me->SetActorLocation(p0 + vt);

	if (currentTime > 1)
	{
		me->Destroy();
	}
}

// 플레이어에게 맞았다.
void UEnemyFSM::OnDamageProcess(int damageValue)
{
	if (ai)
	{
		ai->StopMovement();
	}
	// 체력을 소모하고
	me->hp -= damageValue;
	// 체력이 0이되면
	if (me->hp <= 0)
	{
		me->hp = 0;
		// Die 하고 싶다.
		SetState(EEnemyState::DIE);
		me->enemyAnim->bEnemyDieEnd = false;
		// 몽타주의 Die섹션을 플레이 시키고싶다.
		me->OnMyDamage(TEXT("Die"));

		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 게임모드의 AddExp함수를 호출하고싶다. 1점
		Cast<AMyTPSGameGameModeBase>(GetWorld()->GetAuthGameMode())->AddExp(1);
	}
	// 그렇지 않다면
	else 
	{
		//	Damage 하고 싶다.
		SetState(EEnemyState::DAMAGE);

		int index = FMath::RandRange(0, 1);
		FString sectionName =
			FString::Printf(TEXT("Damage%d"), index);

		me->OnMyDamage(FName(*sectionName));
		// Ctrl + K + C  / Ctrl + K + U
		//if (FMath::RandRange(0, 100) > 50)
		//{
		//	me->OnMyDamage(TEXT("Damage1"));
		//}
		//else
		//{
		//	me->OnMyDamage(TEXT("Damage0"));
		//}
	}
}


void UEnemyFSM::SetState(EEnemyState next)
{
	state = next;
	me->enemyAnim->state = next;
	currentTime = 0;
}

// enemy -> player을 공격하는 순간.
void UEnemyFSM::OnHitEvent()
{
	me->enemyAnim->bAttackPlay = false;

	// 3. 공격을 하고 (조건은 공격거리 안에 있는가?)
	float dist = target->GetDistanceTo(me);
	if (dist <= attakcRange)
	{
		target->OnMyHit(1);
	}
}

bool UEnemyFSM::UpdateRandomLocation(float radius, FVector& outLocation)
{
	UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	FNavLocation navLoc;
	bool result = ns->GetRandomReachablePointInRadius(me->GetActorLocation(), radius, navLoc);
	if (result)
	{
		outLocation = navLoc.Location;
	}

	return result;
}
