// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

#define GRENADE_GUN true
#define SNIPER_GUN false

DECLARE_MULTICAST_DELEGATE_OneParam(FSetupInputDelegate, class UInputComponent*);

class USpringArmComponent; // 전방선언

UCLASS()
class MYTPSGAME_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* springArmComp; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCameraComponent* cameraComp;

	UPROPERTY(EditAnywhere)
	class UTPSPlayerMoveComponent* moveComp;

	UPROPERTY(EditAnywhere)
	class UTPSPlayerFireComponent* fireComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* sniperMeshComp;

	FSetupInputDelegate setupInputDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int hp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int maxHp = 100;

	// enemy->player를 공격함.
	UFUNCTION(BlueprintNativeEvent)
	void OnMyHit(int damage);
};

