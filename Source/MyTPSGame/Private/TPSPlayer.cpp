// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "BulletActor.h"
#include "Blueprint/UserWidget.h"

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
	gunMeshComp->SetupAttachment(GetMesh());
	// 일반총의 에셋을 읽어서 컴포넌트에 넣고싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);
		gunMeshComp->SetRelativeLocationAndRotation(FVector(0, 50, 130), FRotator(0, 0, 0));
	}

	sniperMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperMeshComp"));

	sniperMeshComp->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSniper(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));

	if (tempSniper.Succeeded())
	{
		sniperMeshComp->SetStaticMesh(tempSniper.Object);
		sniperMeshComp->SetRelativeLocationAndRotation(FVector(0, 60, 140), FRotator(0, 0, 0));
		sniperMeshComp->SetRelativeScale3D(FVector(0.15f));
	}

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	// UI를 생성하고싶다.
	crosshairUI = CreateWidget(GetWorld(), crosshairFactory);
	sniperUI = CreateWidget(GetWorld(), sniperFactory);
	// crosshairUI를 화면에 표시하고싶다.
	crosshairUI->AddToViewport();

	ChooseGun(GRENADE_GUN);

	// 1. 태어날 때 cui를 보이게하고싶다.
	// 2. 스나이퍼건일때 ZoomIn을 하면 cui X, sui O
	// 3. 스나이퍼건일때 ZoomOut을 하면 cui O, sui X
	// 4. 기본총을 선택하면 cui O, sui X
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// direction 방향으로 이동하고싶다.

	FTransform trans(GetControlRotation());
	FVector resultDirection = trans.TransformVector(direction);

	resultDirection.Z = 0;
	resultDirection.Normalize();

	AddMovementInput(resultDirection);

	direction = FVector::ZeroVector;

}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::OnAxisHorizontal);

	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::OnAxisVertical);

	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ATPSPlayer::OnAxisLookUp);

	PlayerInputComponent->BindAxis(TEXT("Turn Right"), this, &ATPSPlayer::OnAxisTurnRight);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::OnActionJump);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::OnActionFirePressed);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ATPSPlayer::OnActionFireReleased);

	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATPSPlayer::OnActionGrenade);

	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::OnActionSniper);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &ATPSPlayer::OnActionZoomIn);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &ATPSPlayer::OnActionZoomOut);
}

void ATPSPlayer::OnAxisHorizontal(float value)
{
	direction.Y = value;
}

void ATPSPlayer::OnAxisVertical(float value)
{
	direction.X = value;
}

void ATPSPlayer::OnAxisLookUp(float value)
{
	// Pitch
	AddControllerPitchInput(value);
}

void ATPSPlayer::OnAxisTurnRight(float value)
{
	// Yaw
	AddControllerYawInput(value);
}

void ATPSPlayer::OnActionJump()
{
	Jump();
}

void ATPSPlayer::OnActionFirePressed()
{
	GetWorldTimerManager().SetTimer(fireTimerHandle, this, &ATPSPlayer::DoFire, fireInterval, true);

	DoFire();
}

void ATPSPlayer::OnActionFireReleased()
{
	GetWorldTimerManager().ClearTimer(fireTimerHandle);
}

void ATPSPlayer::DoFire()
{
	// 플레이어 1M앞

	FTransform t = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//t.SetRotation(FQuat(GetControlRotation()));

	GetWorld()->SpawnActor<ABulletActor>(bulletFactory, t);

}

void ATPSPlayer::ChooseGun(bool bGrenade)
{
	// 만약 바꾸기 전이 스나이퍼건이다 그리고 바꾸려는것이 유탄이면
	if (false == bChooseGrenadeGun && true == bGrenade){
		// FOV를 90, cui O sui X
		cameraComp->SetFieldOfView(90);
		crosshairUI->AddToViewport();
		sniperUI->RemoveFromParent();
	}

	bChooseGrenadeGun = bGrenade;

	gunMeshComp->SetVisibility(bGrenade);
	// Not 비트연산
	sniperMeshComp->SetVisibility(!bGrenade);
}

void ATPSPlayer::OnActionGrenade()
{
	ChooseGun(GRENADE_GUN);
}

void ATPSPlayer::OnActionSniper()
{
	ChooseGun(SNIPER_GUN);
}

void ATPSPlayer::OnActionZoomIn()
{
	// 만약 유탄이라면 바로 종료
	if (true == bChooseGrenadeGun)	{
		return;
	}
	// 확대 FOV 30
	cameraComp->SetFieldOfView(30);
	// crosshair를 안보이게하고, 확대경을 보이게하고싶다.
	crosshairUI->RemoveFromParent();
	sniperUI->AddToViewport();
}

void ATPSPlayer::OnActionZoomOut()
{
	// 만약 유탄이라면 바로 종료
	if (true == bChooseGrenadeGun) {
		return;
	}
	// 확대 FOV 90
	cameraComp->SetFieldOfView(90);
	// crosshair를 보이게하고, 확대경을 안보이게하고싶다.
	crosshairUI->AddToViewport();
	sniperUI->RemoveFromParent();
}

