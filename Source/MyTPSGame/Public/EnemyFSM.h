// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8 // 0~255
{
	IDLE,
	MOVE,
	ATTACK,
	DAMAGE,
	DIE,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTPSGAME_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	EEnemyState state;
	class ATPSPlayer* target; // caching

	class AEnemy* me;

	// 공격가능거리 -> 이동하다가 멈추는 조건값
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float attakcRange = 200;

	float currentTime = 0;
	bool bAttackPlay;

	UPROPERTY(EditAnywhere)
	float attackDelayTime = 3;


private:
	void TickIdle();
	void TickMove();
	void TickAttack();
	void TickDamage();
	void TickDie();

public:
	int hp;
	int maxHP = 2;
	void OnDamageProcess(int damageValue); // CallBack 함수, 이벤트 함수


	void SetState(EEnemyState next);

	void OnHitEvent();

	class AAIController* ai;
};


