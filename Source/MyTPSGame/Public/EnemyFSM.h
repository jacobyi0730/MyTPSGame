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

UENUM(BlueprintType)
enum class EEnemyMoveSubState : uint8
{
	PATROL,
	CHASE,
	OLD_MOVE,
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

	EEnemyMoveSubState moveSubState;

	UPROPERTY()
	class ATPSPlayer* target; // caching

	UPROPERTY()
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
	void TickMoveOldMove();
	void TickPatrol();
	void TickChase();
	void TickMove();
	void TickAttack();
	void TickDamage();
	void TickDie();

public:
	void OnDamageProcess(int damageValue); // CallBack 함수, 이벤트 함수


	void SetState(EEnemyState next);

	void OnHitEvent();

	class AAIController* ai;

	UPROPERTY(EditAnywhere)
	float randLocationRadius = 500;

	FVector randomLocation;

	bool UpdateRandomLocation(float radius, FVector& outLocation);


	// PathManager를 알고싶다.
	UPROPERTY()
	class APathManager* pathManager;
	// PathManager의 waypoints를 이용해서 순찰할 목적지를 정하고싶다.
	int wayIndex;

};


