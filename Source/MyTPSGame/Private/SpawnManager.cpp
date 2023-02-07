// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoint.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), spawnList);

	// 랜덤한 시간후에 적생성 함수를 호출하고싶다.
	int randTime = FMath::RandRange(minTime, maxTime);
	GetWorldTimerManager().SetTimer(timerHandleMakeEnemy, this, &ASpawnManager::MakeEnemy, randTime, false);
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnManager::MakeEnemy()
{
	// 적공장에서 적을 생성하고싶다.
	// 어디에? 랜덤한 위치에
	// 만약에 선택한 인덱스가 이전 인덱스와 같다면 다시 값을 정하고싶다.
	int rIndex = 0;
	
	rIndex = FMath::RandRange(0, spawnList.Num() - 1);
	if (rIndex == prevRandIndex)
	{
		rIndex = (rIndex + 1) % spawnList.Num();
	}
	
	prevRandIndex = rIndex;

	FVector loc = spawnList[rIndex]->GetActorLocation();
	FRotator rot = spawnList[rIndex]->GetActorRotation();
	GetWorld()->SpawnActor<AEnemy>(enemyFactory, loc, rot);

	int randTime = FMath::RandRange(minTime, maxTime);
	GetWorldTimerManager().SetTimer(timerHandleMakeEnemy, this, &ASpawnManager::MakeEnemy, randTime, false);
}

