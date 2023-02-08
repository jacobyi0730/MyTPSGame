// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

UCLASS()
class MYTPSGAME_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 랜덤시간마다 적을 랜덤위치에 생성하고싶다.
	// - 랜덤위치를 위한 위치목록
	UPROPERTY(EditAnywhere)
	TArray<AActor*> spawnList;
	// 이전에 선택한 랜덤 위치
	int prevRandIndex = -1;
	// - 최소, 최대시간
	UPROPERTY(EditAnywhere)
	float minTime = 1;
	UPROPERTY(EditAnywhere)
	float maxTime = 2;
	// 생성할시간
	float createTime;
	// - 타이머핸들
	FTimerHandle timerHandleMakeEnemy;

	// - 적공장 -> 언리얼객체
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemy> enemyFactory1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemy> enemyFactory2;


	void MakeEnemy();

	// 만든 갯수
	int makeCount;
	// 목표 갯수
	int makeTargetCount;

};
