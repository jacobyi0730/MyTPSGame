// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerFireComponent.h"
#include "BulletActor.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "TPSPlayerAnim.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTPSPlayerFireComponent::UTPSPlayerFireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USoundBase> tempFireSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));

	if (tempFireSound.Succeeded())
	{
		fireSound = tempFireSound.Object;
	}
}


// Called when the game starts
void UTPSPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<ATPSPlayer>(GetOwner());

	// UI�� �����ϰ�ʹ�.
	crosshairUI = CreateWidget(GetWorld(), crosshairFactory);
	sniperUI = CreateWidget(GetWorld(), sniperFactory);
	// crosshairUI�� ȭ�鿡 ǥ���ϰ�ʹ�.
	crosshairUI->AddToViewport();

	ChooseGun(GRENADE_GUN);

	gunAmmo = maxGunAmmo;
	sniperAmmo = maxSniperAmmo;
}


// Called every frame
void UTPSPlayerFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTPSPlayerFireComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionFirePressed);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &UTPSPlayerFireComponent::OnActionFireReleased);

	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionGrenade);

	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionSniper);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionZoomIn);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &UTPSPlayerFireComponent::OnActionZoomOut);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionReload);
}



void UTPSPlayerFireComponent::OnMyGunReload()
{
	gunAmmo = maxGunAmmo;
}

void UTPSPlayerFireComponent::OnMySniperReload()
{
	sniperAmmo = maxSniperAmmo;
}


void UTPSPlayerFireComponent::OnActionFirePressed()
{
	// ���� �� �Ѿ��� �����ִ��� �����ϰ�ʹ�.
	// ���� �����ִٸ� 1�� �����ϰ�ʹ�.
	// �׷��� ������ ���� ���� �ʰڴ�...
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


	// ī�޶� ����ʹ�.
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// ���� �̹� ���� �־��ٸ�
	// -> ���� canShakeInstance�� nullptr�� �ƴϴ�. �׸��� ���� ���̶��
	if (nullptr != canShakeInstance && false == canShakeInstance->IsFinished())
	{
		// ����ϰ�
		cameraManager->StopCameraShake(canShakeInstance);
	}
	// ����ʹ�.
	canShakeInstance = cameraManager->StartCameraShake(camShakeFactory);


	// �ѽ�� �ִϸ��̼��� ����ϰ�ʹ�.
	auto anim = Cast<UTPSPlayerAnim>(me->GetMesh()->GetAnimInstance());
	anim->OnFire(TEXT("Default"));

	// �ѼҸ��� ����ʹ�.
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, me->GetActorLocation(), me->GetActorRotation());

	// ���� �⺻���̶��
	if (bChooseGrenadeGun)
	{
		me->GetWorldTimerManager().SetTimer(fireTimerHandle, this, &UTPSPlayerFireComponent::DoFire, fireInterval, true);

		DoFire();
	}
	// �׷����ʴٸ� (�������۰�)
	else
	{
		FHitResult hitInfo;
		FVector start = me->cameraComp->GetComponentLocation(); // ī�޶��� ������ǥ
		FVector end = start + me->cameraComp->GetForwardVector() * 100000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(me);

		// �ٶ󺸰�ʹ�.
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECollisionChannel::ECC_Visibility, params);

		// ���� �ε��� ���� �ִٸ�
		if (true == bHit)
		{
			// ��ȣ�ۿ��� �ϰ�ʹ�.

			// �ε��� ���� ��������Ʈ�� ǥ���ϰ�ʹ�.
			FTransform trans(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, trans);

			// ���� �ε��� ���Ͱ� Enemy���
			auto hitActor = hitInfo.GetActor();
			AEnemy* enemy = Cast<AEnemy>(hitActor);
			if (nullptr != enemy)
			{
				// Enemy���� �������� �ְ�ʹ�.
				UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("enemyFSM")));

				fsm->OnDamageProcess(1);


			}

			auto hitComp = hitInfo.GetComponent();
			// �ε��� ��ü�� �����ۿ��� �ϰ��ִٸ�
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// ���� ���ϰ�ʹ�.
				FVector forceDir = (hitInfo.TraceEnd - hitInfo.TraceStart).GetSafeNormal();

				FVector force = forceDir * 1000000 * hitComp->GetMass();

				hitComp->AddForce(force);
			}
		}




	}
}

void UTPSPlayerFireComponent::OnActionFireReleased()
{
	me->GetWorldTimerManager().ClearTimer(fireTimerHandle);
}

void UTPSPlayerFireComponent::DoFire()
{
	// �÷��̾� 1M��

	FTransform t = me->gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//t.SetRotation(FQuat(GetControlRotation()));

	GetWorld()->SpawnActor<ABulletActor>(bulletFactory, t);

}

void UTPSPlayerFireComponent::ChooseGun(bool bGrenade)
{
	// ���� �ٲٱ� ���� �������۰��̴� �׸��� �ٲٷ��°��� ��ź�̸�
	if (false == bChooseGrenadeGun && true == bGrenade) {
		// FOV�� 90, cui O sui X
		me->cameraComp->SetFieldOfView(90);
		crosshairUI->AddToViewport();
		sniperUI->RemoveFromParent();
	}

	bChooseGrenadeGun = bGrenade;

	me->gunMeshComp->SetVisibility(bGrenade);
	// Not ��Ʈ����
	me->sniperMeshComp->SetVisibility(!bGrenade);
}

void UTPSPlayerFireComponent::OnActionGrenade()
{
	ChooseGun(GRENADE_GUN);
}

void UTPSPlayerFireComponent::OnActionSniper()
{
	ChooseGun(SNIPER_GUN);
}

void UTPSPlayerFireComponent::OnActionZoomIn()
{
	// ���� ��ź�̶�� �ٷ� ����
	if (true == bChooseGrenadeGun) {
		return;
	}
	// Ȯ�� FOV 30
	me->cameraComp->SetFieldOfView(30);
	// crosshair�� �Ⱥ��̰��ϰ�, Ȯ����� ���̰��ϰ�ʹ�.
	crosshairUI->RemoveFromParent();
	sniperUI->AddToViewport();
}

void UTPSPlayerFireComponent::OnActionZoomOut()
{
	// ���� ��ź�̶�� �ٷ� ����
	if (true == bChooseGrenadeGun) {
		return;
	}
	// Ȯ�� FOV 90
	me->cameraComp->SetFieldOfView(90);
	// crosshair�� ���̰��ϰ�, Ȯ����� �Ⱥ��̰��ϰ�ʹ�.
	crosshairUI->AddToViewport();
	sniperUI->RemoveFromParent();
}


void UTPSPlayerFireComponent::OnActionReload()
{
	auto anim = Cast<UTPSPlayerAnim>(me->GetMesh()->GetAnimInstance());
	if (bChooseGrenadeGun)
	{
		anim->OnFire(TEXT("GunReload"));
	}
	else
	{
		anim->OnFire(TEXT("SniperReload"));
	}
}

