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


	ConstructorHelpers::FObjectFinder<USoundBase> tempFireSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));

	if (tempFireSound.Succeeded())
	{
		fireSound = tempFireSound.Object;
	}

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 태어날 때 속력을 speedWalk로 설정하고싶다.
	GetCharacterMovement()->MaxWalkSpeed = speedWalk;

	// UI를 생성하고싶다.
	crosshairUI = CreateWidget(GetWorld(), crosshairFactory);
	sniperUI = CreateWidget(GetWorld(), sniperFactory);
	// crosshairUI를 화면에 표시하고싶다.
	crosshairUI->AddToViewport();

	ChooseGun(GRENADE_GUN);

	gunAmmo = maxGunAmmo;
	sniperAmmo = maxSniperAmmo;

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


	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::OnActionRunPressed);

	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::OnActionRunReleased);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ATPSPlayer::OnActionCrouchPressed);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ATPSPlayer::OnActionCrouchReleased);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ATPSPlayer::OnActionReload);
}

void ATPSPlayer::OnMyGunReload()
{
	gunAmmo = maxGunAmmo;
}

void ATPSPlayer::OnMySniperReload()
{
	sniperAmmo = maxSniperAmmo;
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

void ATPSPlayer::OnActionRunPressed()
{
	GetCharacterMovement()->MaxWalkSpeed = speedRun;
}
void ATPSPlayer::OnActionRunReleased()
{
	GetCharacterMovement()->MaxWalkSpeed = speedWalk;
}
void ATPSPlayer::OnActionCrouchPressed()
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = speedCrouch;
	Crouch();
}
void ATPSPlayer::OnActionCrouchReleased()
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = speedWalk;
	UnCrouch();
}

void ATPSPlayer::OnActionFirePressed()
{
	// 총을 쏠때 총알이 남아있는지 검증하고싶다.
	// 만약 남아있다면 1발 차감하고싶다.
	// 그렇지 않으면 총을 쏘지 않겠다...
	if (bChooseGrenadeGun)
	{
		if (gunAmmo > 0) { gunAmmo--; }
		else { return; }
	}
	else
	{
		if (sniperAmmo > 0) { sniperAmmo--; }
		else { return; }
	}


	// 카메라를 흔들고싶다.
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// 만약 이미 흔들고 있었다면
	// -> 만약 canShakeInstance가 nullptr이 아니다. 그리고 흔드는 중이라면
	if (nullptr != canShakeInstance && false == canShakeInstance->IsFinished())
	{
		// 취소하고
		cameraManager->StopCameraShake(canShakeInstance);
	}
	// 흔들고싶다.
	canShakeInstance = cameraManager->StartCameraShake(camShakeFactory);


	// 총쏘는 애니메이션을 재생하고싶다.
	auto anim = Cast<UTPSPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->OnFire(TEXT("Default"));

	// 총소리를 내고싶다.
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation(), GetActorRotation());

	// 만약 기본총이라면
	if (bChooseGrenadeGun)
	{
		GetWorldTimerManager().SetTimer(fireTimerHandle, this, &ATPSPlayer::DoFire, fireInterval, true);

		DoFire();
	}
	// 그렇지않다면 (스나이퍼건)
	else
	{
		FHitResult hitInfo;
		FVector start = cameraComp->GetComponentLocation(); // 카메라의 월드좌표
		FVector end = start + cameraComp->GetForwardVector() * 100000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		// 바라보고싶다.
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECollisionChannel::ECC_Visibility, params);

		// 만약 부딪힌 것이 있다면
		if (true == bHit)
		{
			// 상호작용을 하고싶다.

			// 부딪힌 곳에 폭발이펙트를 표시하고싶다.
			FTransform trans(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, trans);

			// 만약 부딪힌 액터가 Enemy라면
			auto hitActor = hitInfo.GetActor();
			AEnemy* enemy = Cast<AEnemy>(hitActor);
			if (nullptr != enemy)
			{
				// Enemy에게 데미지를 주고싶다.
				UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("enemyFSM")));

				fsm->OnDamageProcess(1);


			}

			auto hitComp = hitInfo.GetComponent();
			// 부딪힌 물체가 물리작용을 하고있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 힘을 가하고싶다.
				FVector forceDir = (hitInfo.TraceEnd - hitInfo.TraceStart).GetSafeNormal();

				FVector force = forceDir * 1000000 * hitComp->GetMass();

				hitComp->AddForce(force);
			}
		}




	}
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
	if (false == bChooseGrenadeGun && true == bGrenade) {
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
	if (true == bChooseGrenadeGun) {
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

void ATPSPlayer::CallByBlueprint()
{

}

void ATPSPlayer::OnActionReload()
{
	auto anim = Cast<UTPSPlayerAnim>(GetMesh()->GetAnimInstance());
	if (bChooseGrenadeGun)
	{
		anim->OnFire(TEXT("GunReload"));
	}
	else
	{
		anim->OnFire(TEXT("SniperReload"));
	}
}

