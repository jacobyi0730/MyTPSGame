// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "BulletActor.h"
#include "Blueprint/UserWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "EnemyFSM.h"
#include "TPSPlayerAnim.h"
#include "TPSPlayerFireComponent.h"
#include "TPSPlayerMoveComponent.h"
#include "Components/ProgressBar.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// sudo code 의사코드 => 알고리즘
	// 1. 외관에 해당하는 에셋을 읽어오고싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	// 2. 읽어왔을때 성공했다면
	if (tempMesh.Succeeded())
	{
		// 3. Mesh에 적용하고싶다. 
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		// 4. Transform 을 수정하고싶다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// 스프링암, 카메라 컴포넌트를 생성하고싶다.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	// 스프링암을 루트에 붙이고
	springArmComp->SetupAttachment(RootComponent);
	// 카메라는 스프링암에 붙이고싶다.
	cameraComp->SetupAttachment(springArmComp);

	springArmComp->SetRelativeLocation(FVector(0, 50, 100));
	springArmComp->TargetArmLength = 250;

	// 입력값을 회전에 반영하고싶다.
	bUseControllerRotationYaw = true;
	springArmComp->bUsePawnControlRotation = true;
	cameraComp->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 일반총의 컴포넌트를 만들고싶다.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	// 일반총의 에셋을 읽어서 컴포넌트에 넣고싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);
		gunMeshComp->SetRelativeLocationAndRotation(FVector(-5.86f, 1.54f, -2.18f), FRotator(10, 100, -10));
	}

	sniperMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperMeshComp"));

	sniperMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSniper(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	if (tempSniper.Succeeded())
	{
		sniperMeshComp->SetStaticMesh(tempSniper.Object);
		sniperMeshComp->SetRelativeLocationAndRotation(FVector(0, 60, 140), FRotator(0, 0, 0));
		sniperMeshComp->SetRelativeScale3D(FVector(0.15f));
	}
	
	// 이동컴포넌트와 총쏘기컴포넌트를 생성하고싶다.
	moveComp = CreateDefaultSubobject<UTPSPlayerMoveComponent>(TEXT("moveComp"));

	fireComp = CreateDefaultSubobject<UTPSPlayerFireComponent>(TEXT("fireComp"));

	//GetCharacterMovement()->Crouch();
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	hp = maxHp;
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	setupInputDelegate.Broadcast(PlayerInputComponent);
	//moveComp->SetupPlayerInput(PlayerInputComponent);
	//fireComp->SetupPlayerInput(PlayerInputComponent);
}

void ATPSPlayer::OnMyHit_Implementation(int damage)
{
	hp -= damage;

	// 만약 체력이 0이하라면
	if (hp <= 0)
	{
		// 게임오버 처리 하고싶다.
		OnMyGameOver();
	}
}


