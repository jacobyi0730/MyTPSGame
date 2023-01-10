// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyTPSGameGameModeBase.h"
#include "MyTPSGame.h"

AMyTPSGameGameModeBase::AMyTPSGameGameModeBase()
{
	// 로그 출력 Hello World
	//UE_LOG(LogTemp, Warning, TEXT("안녕"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO);
	//PRINT_CALLINFO();
	PRINT_LOG(TEXT("%s %s"), TEXT("Hello"), TEXT("World"));
}