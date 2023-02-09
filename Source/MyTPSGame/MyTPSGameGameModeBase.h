// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyTPSGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MYTPSGAME_API AMyTPSGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyTPSGameGameModeBase();

	virtual void BeginPlay() override;

	UPROPERTY()
	class ASpawnManager* spawnManager;

	// 레벨에 등장할 적의 수는 레벨과 같다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int exp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int needExp = 1;

	void LevelUp();
	void AddExp(int amount);
	void LevelUpProcess();
};
