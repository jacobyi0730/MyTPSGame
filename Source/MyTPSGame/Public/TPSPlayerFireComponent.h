// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerBaseComponent.h"
#include "TPSPlayerFireComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTPSGAME_API UTPSPlayerFireComponent : public UTPSPlayerBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSPlayerFireComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;


	void OnMyGunReload();
	void OnMySniperReload();
	void OnActionFirePressed();
	void OnActionFireReleased();
	void DoFire();

	// 마우스 왼쪽버튼을 누르면 총알공장에서 총알을 만들어서 총구위치에 배치하고싶다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletActor> bulletFactory;
	

	
	UPROPERTY(EditAnywhere)
	float fireInterval = 0.5f;
	
	FTimerHandle fireTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bChooseGrenadeGun;
	
	void ChooseGun(bool bGrenade);

	// 위젯공장에서 위젯을 생성하고싶다. Crosshair, Sniper
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> crosshairFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> sniperFactory;

	UPROPERTY()
	class UUserWidget* crosshairUI;
	UPROPERTY()
	class UUserWidget* sniperUI;

	// 1번키와 2번키에대한 (총교체)입력처리를 하고싶다.
	void OnActionGrenade();
	void OnActionSniper();

	// Zoom을 구현하고싶다. In / Out
	void OnActionZoomIn();	// 확대 FOV 30
	void OnActionZoomOut();	// 축소 FOV 90

	// 폭발공장
	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletImpactFactory;

	
	UPROPERTY(EditAnywhere)
	class USoundBase* fireSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> camShakeFactory;

	UPROPERTY()
	class UCameraShakeBase* canShakeInstance;

	// 탄창, 리로드 등등..

	void OnActionReload();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int maxGunAmmo = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int gunAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int maxSniperAmmo = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int sniperAmmo;

		
};
