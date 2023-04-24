// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGroundedState.h"

#include "PlayerAirborneState.h"
#include "PlayerBaseStateComp.h"
#include "PlayerPawn3D.h"

void UPlayerGroundedState::Enter(APlayerPawn3D* PlayerPtr)
{
	PlayerPawnState::Enter(PlayerPtr);
	
	BaseState = Player->FindComponentByClass<UPlayerBaseStateComp>();
	BaseState->Enter(PlayerPtr);
	BaseState->SetUpInput(InputComp);

	bJump = false;
	Player->InputComponent->BindAction("Jump", IE_Pressed, this, &UPlayerGroundedState::JumpInput);
}

void UPlayerGroundedState::Update(const float DeltaTime)
{
	// Super::Update(DeltaTime);
	BaseState->Update(DeltaTime);
	
	if(bJump)
		Jump();

	if(!BaseState->bIsGrounded)
		Player->SwitchState(Player->AirborneState);
		
	UE_LOG(LogTemp, Warning, TEXT("Grounded"))
}

void UPlayerGroundedState::JumpInput()
{
	bJump = true; 
}

void UPlayerGroundedState::Jump()
{
	if(BaseState->bIsGrounded)
	{
		BaseState->Velocity += FVector::UpVector * JumpDistance;
		Player->SwitchState(Player->AirborneState);
	}

	bJump = false; 
}

void UPlayerGroundedState::ApplyFriction(const float NormalMagnitude) const
{
	// if applied friction is larger than velocity, velocity is set to zero
	if(BaseState->Velocity.Size() < NormalMagnitude * StaticFrictionCoefficient)
		BaseState->Velocity = FVector::ZeroVector; 
	else
		BaseState->Velocity -= BaseState->Velocity.GetSafeNormal() * NormalMagnitude * KineticFrictionCoefficient; 
}
