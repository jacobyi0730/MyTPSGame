// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSPlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class MYTPSGAME_API UTPSPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
public:

	void NativeUpdateAnimation(float DeltaSeconds) override;

	// forwardVelocity, rightVelocity, isInAir
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float forwardVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float rightVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isInAir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isCrouching;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* fireMontageFactory;

	UFUNCTION(BlueprintCallable)
	void OnFire(FName sectionName);

	UFUNCTION(BlueprintCallable)
	void OnGunReload();

	UFUNCTION(BlueprintCallable)
	void OnSniperReload();
};
