// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include "../MyTPSGame.h"

// Sets default values
ABulletActor::ABulletActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체, 외관, 이동컴포넌트
	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(13);
	sphereComp->SetCollisionProfileName(TEXT("BlockAll"));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeScale3D(FVector(0.25f));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComp"));
	movementComp->SetUpdatedComponent(sphereComp);
	movementComp->InitialSpeed = 5000;
	movementComp->MaxSpeed = 5000;
	movementComp->bShouldBounce = true;
	movementComp->Bounciness = 0.5f;
}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
	
	// InitialLifeSpan = 2;
	// SetLifeSpan(2);
	FTimerHandle dieTimerHandle;
	//GetWorldTimerManager().SetTimer(dieTimerHandle, this, &ABulletActor::OnDie, 0.1f);

	GetWorldTimerManager().SetTimer(dieTimerHandle, FTimerDelegate::CreateLambda(
		[this]()->void {
			this->Destroy();
		}
	), 2, false);


	// Lambda 함수
	// 캡쳐 
	//int number = 1;
	//auto myPrint = [&number]()->void {
	//	int b = number;
	//};

	//myPrint();


	//auto myPlus = [this](int a, int b)->int{ return a + b; };

	//PRINT_LOG(TEXT("%d"), myPlus(10, 20));


	
	
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletActor::OnDie()
{
	Destroy();
}

