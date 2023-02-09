// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerMoveComponent.h"

#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UTPSPlayerMoveComponent::UTPSPlayerMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTPSPlayerMoveComponent::BeginPlay()
{
	Super::BeginPlay();


	// 태어날 때 속력을 speedWalk로 설정하고싶다.
	me->GetCharacterMovement()->MaxWalkSpeed = speedWalk;
}


// Called every frame
void UTPSPlayerMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// direction 방향으로 이동하고싶다.
	FTransform trans(me->GetControlRotation());
	FVector resultDirection = trans.TransformVector(direction);
	resultDirection.Z = 0;
	resultDirection.Normalize();

	me->AddMovementInput(resultDirection);

	direction = FVector::ZeroVector;
}

void UTPSPlayerMoveComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &UTPSPlayerMoveComponent::OnAxisHorizontal);

	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &UTPSPlayerMoveComponent::OnAxisVertical);

	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &UTPSPlayerMoveComponent::OnAxisLookUp);

	PlayerInputComponent->BindAxis(TEXT("Turn Right"), this, &UTPSPlayerMoveComponent::OnAxisTurnRight);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UTPSPlayerMoveComponent::OnActionJump);

	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &UTPSPlayerMoveComponent::OnActionRunPressed);

	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &UTPSPlayerMoveComponent::OnActionRunReleased);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &UTPSPlayerMoveComponent::OnActionCrouchPressed);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &UTPSPlayerMoveComponent::OnActionCrouchReleased);
}


void UTPSPlayerMoveComponent::OnAxisHorizontal(float value)
{
	direction.Y = value;
}

void UTPSPlayerMoveComponent::OnAxisVertical(float value)
{
	direction.X = value;
}

void UTPSPlayerMoveComponent::OnAxisLookUp(float value)
{
	// Pitch
	me->AddControllerPitchInput(value);
}

void UTPSPlayerMoveComponent::OnAxisTurnRight(float value)
{
	// Yaw
	me->AddControllerYawInput(value);
}

void UTPSPlayerMoveComponent::OnActionJump()
{
	me->Jump();
}

void UTPSPlayerMoveComponent::OnActionRunPressed()
{
	me->GetCharacterMovement()->MaxWalkSpeed = speedRun;
}
void UTPSPlayerMoveComponent::OnActionRunReleased()
{
	me->GetCharacterMovement()->MaxWalkSpeed = speedWalk;
}
void UTPSPlayerMoveComponent::OnActionCrouchPressed()
{
	me->GetCharacterMovement()->MaxWalkSpeedCrouched = speedCrouch;
	me->Crouch();
}
void UTPSPlayerMoveComponent::OnActionCrouchReleased()
{
	me->GetCharacterMovement()->MaxWalkSpeedCrouched = speedWalk;
	me->UnCrouch();
}


