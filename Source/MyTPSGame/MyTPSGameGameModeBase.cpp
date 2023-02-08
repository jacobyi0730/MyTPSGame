// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyTPSGameGameModeBase.h"
#include "MyTPSGame.h"
#include "SpawnManager.h"
#include "Kismet/GameplayStatics.h"

AMyTPSGameGameModeBase::AMyTPSGameGameModeBase()
{
	// 로그 출력 Hello World
	//UE_LOG(LogTemp, Warning, TEXT("안녕"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO);
	//PRINT_CALLINFO();
	PRINT_LOG(TEXT("%s %s"), TEXT("Hello"), TEXT("World"));
}

void AMyTPSGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	spawnManager = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));

	LevelUp();
}

void AMyTPSGameGameModeBase::LevelUp()
{
	level++;
	// 스폰매니저의 makeTargetCount를 현재 레벨로 설정하고싶다.
	spawnManager->makeTargetCount = level;

	spawnManager->makeCount = 0;

	// needExp도 설정하고싶다.
	needExp = level;
}

void AMyTPSGameGameModeBase::AddExp(int amount)
{
	exp += amount;

	// 경험치가 레벨업에 도달했는가?
	if (exp >= needExp)
	{
		// 레벨업 시작
		LevelUpProcess();
	}
}

void AMyTPSGameGameModeBase::LevelUpProcess()
{
	// 이펙트 표시
	exp -= needExp;
	LevelUp();

	if (exp >= needExp)
	{
		// 레벨업 반복
		FTimerHandle th;
		GetWorldTimerManager().SetTimer(th, this, &AMyTPSGameGameModeBase::LevelUpProcess, 0.2f, false);
	}
}
