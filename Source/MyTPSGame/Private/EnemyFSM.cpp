// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "TPSPlayer.h"
#include "../MyTPSGame.h"
#include <Components/CapsuleComponent.h>

#include "EnemyAnim.h"

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

	// me를 찾고싶다.
	me = Cast<AEnemy>(GetOwner());

	// 태어날 때 현재 체력을 최대 체력으로 하고싶다.
	hp = maxHP;

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

// 목적지를 향해서 이동하고싶다.
// 목적지와의 거리가 공격가능거리라면
// 공격상태로 전이하고싶다.
void UEnemyFSM::TickMove()
{
	
	// 1. 목적지를 향하는 방향을 만들고
	FVector dir = target->GetActorLocation() - me->GetActorLocation();
	// 2. 그 방향으로 이동하고싶다.
	me->AddMovementInput(dir.GetSafeNormal());
	// 3. 목적지와의 거리가 공격가능거리라면
	float dist = dir.Size();
	//float dist = target->GetDistanceTo(me);
	//float dist = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());
	if (dist <= attakcRange) {
		// 4. 공격상태로 전이하고싶다.
		SetState(EEnemyState::ATTACK);
	}
}

// 공격 타이밍 
void UEnemyFSM::TickAttack()
{
	// 1. 시간이 흐르다가
	currentTime += GetWorld()->GetDeltaSeconds();
	// 2. 현재시간이 공격시간을 초과하면
	if (false == bAttackPlay && currentTime > 0.1f)
	{
		bAttackPlay = true;
		// 3. 공격을 하고 (조건은 공격거리 안에 있는가?)
		float dist = target->GetDistanceTo(me);
		if (dist <= attakcRange)
		{
			PRINT_LOG(TEXT("Enemy is Attack"));
		}
	}
	// 4. 공격동작이 끝났다면
	if (currentTime > 2)
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
	// 체력을 소모하고
	hp -= damageValue;
	// 체력이 0이되면
	if (hp <= 0) {
		// Die 하고 싶다.
		SetState(EEnemyState::DIE);
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	// 그렇지 않다면
	else {
		//	Damage 하고 싶다.
		SetState(EEnemyState::DAMAGE);
	}
}


void UEnemyFSM::SetState(EEnemyState next)
{
	state = next;
	me->enemyAnim->state = next;
}
