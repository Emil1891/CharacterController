// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGroundedState.h"

#include "PlayerAirborneState.h"
#include "PlayerPawn3D.h"

void UPlayerGroundedState::Enter(APlayerPawn3D* PlayerPtr)
{
	Super::Enter(PlayerPtr);

	bJump = false;
	InputComp->BindAction("Jump", IE_Pressed, this, &UPlayerGroundedState::JumpInput);
}

void UPlayerGroundedState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);
	
	if(bJump)
		Jump();
}

void UPlayerGroundedState::JumpInput()
{
	bJump = true; 
}

void UPlayerGroundedState::Jump()
{
	if(bIsGrounded)
	{
		Velocity += FVector::UpVector * JumpDistance;
		Player->SwitchState(Player->AirborneState);
	}

	bJump = false; 
}


void UPlayerGroundedState::ApplyFriction(const float NormalMagnitude)
{
	// if applied friction is larger than velocity, velocity is set to zero
	if(Velocity.Size() < NormalMagnitude * StaticFrictionCoefficient)
		Velocity = FVector::ZeroVector; 
	else
		Velocity -= Velocity.GetSafeNormal() * NormalMagnitude * KineticFrictionCoefficient; 
}
