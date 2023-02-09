// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPlayerFireComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTPSGAME_API UTPSPlayerFireComponent : public UActorComponent
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

	void SetupPlayerInput(class UInputComponent* PlayerInputComponent);

	UPROPERTY()
	class ATPSPlayer* me;


	void OnMyGunReload();
	void OnMySniperReload();
	void OnActionFirePressed();
	void OnActionFireReleased();
	void DoFire();

	// ���콺 ���ʹ�ư�� ������ �Ѿ˰��忡�� �Ѿ��� ���� �ѱ���ġ�� ��ġ�ϰ�ʹ�.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletActor> bulletFactory;
	

	
	UPROPERTY(EditAnywhere)
	float fireInterval = 0.5f;
	
	FTimerHandle fireTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bChooseGrenadeGun;
	
	void ChooseGun(bool bGrenade);

	// �������忡�� ������ �����ϰ�ʹ�. Crosshair, Sniper
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> crosshairFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> sniperFactory;

	UPROPERTY()
	class UUserWidget* crosshairUI;
	UPROPERTY()
	class UUserWidget* sniperUI;

	// 1��Ű�� 2��Ű������ (�ѱ�ü)�Է�ó���� �ϰ�ʹ�.
	void OnActionGrenade();
	void OnActionSniper();

	// Zoom�� �����ϰ�ʹ�. In / Out
	void OnActionZoomIn();	// Ȯ�� FOV 30
	void OnActionZoomOut();	// ��� FOV 90

	// ���߰���
	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletImpactFactory;

	
	UPROPERTY(EditAnywhere)
	class USoundBase* fireSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> camShakeFactory;

	UPROPERTY()
	class UCameraShakeBase* canShakeInstance;

	// źâ, ���ε� ���..

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
