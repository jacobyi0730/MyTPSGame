// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "TPSPlayer.h"
#include "../MyTPSGame.h"
#include <Components/CapsuleComponent.h>

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

	state = EEnemyState::IDLE; // �����, �Ͻ���

	// me�� ã��ʹ�.
	me = Cast<AEnemy>(GetOwner());
	
	// �¾ �� ���� ü���� �ִ� ü������ �ϰ�ʹ�.
	hp = maxHP;
	
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
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

//  ���, �÷��̾ ã���� �̵����� ����
void UEnemyFSM::TickIdle()
{	
	// 1. �÷��̾ ã��ʹ�.
	target = Cast<ATPSPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	// 2. ���� �÷��̾ ã������
	if (nullptr != target)
	{
		// 3. �̵����� �����ϰ�ʹ�.
		state = EEnemyState::MOVE;
	}
}

// �������� ���ؼ� �̵��ϰ�ʹ�.
// ���������� �Ÿ��� ���ݰ��ɰŸ����
// ���ݻ��·� �����ϰ�ʹ�.
void UEnemyFSM::TickMove()
{
	// 1. �������� ���ϴ� ������ �����
	FVector dir = target->GetActorLocation() - me->GetActorLocation();
	// 2. �� �������� �̵��ϰ�ʹ�.
	me->AddMovementInput(dir.GetSafeNormal());
	// 3. ���������� �Ÿ��� ���ݰ��ɰŸ����
	float dist = dir.Size();
	//float dist = target->GetDistanceTo(me);
	//float dist = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());
	if (dist <= attakcRange) {
		// 4. ���ݻ��·� �����ϰ�ʹ�.
		state = EEnemyState::ATTACK;
	}
}

// ���� Ÿ�̹� 
void UEnemyFSM::TickAttack()
{
	// 1. �ð��� �帣�ٰ�
	currentTime += GetWorld()->GetDeltaSeconds();
	// 2. ����ð��� ���ݽð��� �ʰ��ϸ�
	if (false == bAttackPlay && currentTime > 0.1f)
	{
		bAttackPlay = true;
		// 3. ������ �ϰ� (������ ���ݰŸ� �ȿ� �ִ°�?)
		float dist = target->GetDistanceTo(me);
		if (dist <= attakcRange) 
		{
			PRINT_LOG(TEXT("Enemy is Attack"));
		}
	}
	// 4. ���ݵ����� �����ٸ�
	if (currentTime > 2)
	{
		// 5. ��� ������ �� ������ �Ǵ��ϰ�ʹ�.
		// �Ÿ��� ���ϰ�
		float dist = target->GetDistanceTo(me);
		// ���ݰŸ����� �־����ٸ�(�������ٸ�)
		if (dist > attakcRange)
		{
			// �̵����·� �����ϰ�ʹ�.
			state = EEnemyState::MOVE;
		}
		else { // ���ݰŸ� �ȿ� ������ ����ؼ� �����ϰ�ʹ�.
			currentTime = 0;
			bAttackPlay = false;
		}
	}
}

// player->enemy ����
void UEnemyFSM::TickDamage()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	if (currentTime > 1)
	{
		state = EEnemyState::MOVE;
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

// �÷��̾�� �¾Ҵ�.
void UEnemyFSM::OnDamageProcess(int damageValue)
{
	// ü���� �Ҹ��ϰ�
	hp -= damageValue;
	// ü���� 0�̵Ǹ�
	if (hp <= 0) {
		// Die �ϰ� �ʹ�.
		state = EEnemyState::DIE;
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	// �׷��� �ʴٸ�
	else {
		//	Damage �ϰ� �ʹ�.
		state = EEnemyState::DAMAGE;
	}
}

