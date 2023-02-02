// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerAnim.h"

#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTPSPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Owner의 Velocity를 가져와서
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());

	if (nullptr == owner)
	{
		return;
	}
	// rightVelocity, forwardVelocity를 구하고싶다.
	// Vector의 내적을 이용해서 방향값을 구하고싶다.
	FVector velocity = owner->GetVelocity();
	rightVelocity = FVector::DotProduct(velocity, owner->GetActorRightVector());

	forwardVelocity = FVector::DotProduct(velocity, owner->GetActorForwardVector());
		// isInAir를 구하고싶다.
	
	isInAir = owner->GetCharacterMovement()->IsFalling();
}

void UTPSPlayerAnim::OnFire(FName sectionName)
{
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());
	owner->PlayAnimMontage(fireMontageFactory, 1, sectionName);
	//Montage_Play(fireMontageFactory);
}

void UTPSPlayerAnim::OnGunReload()
{
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());
	owner->OnMyGunReload();
}

void UTPSPlayerAnim::OnSniperReload()
{
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());
	owner->OnMySniperReload();
}
