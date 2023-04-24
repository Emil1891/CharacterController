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

	// binds jump input 
	Player->InputComponent->BindAction("Jump", IE_Pressed, this, &UPlayerGroundedState::Jump);
}

void UPlayerGroundedState::Update(const float DeltaTime)
{
	BaseState->Update(DeltaTime);

	// becomes airborne without jumping i.e walking off a ledge 
	if(!BaseState->bIsGrounded)
		Player->SwitchState(Player->AirborneState);
		
	UE_LOG(LogTemp, Warning, TEXT("Grounded"))
}

void UPlayerGroundedState::Jump() 
{
	BaseState->Velocity += FVector::UpVector * JumpDistance;
	// changes state to airborne 
	Player->SwitchState(Player->AirborneState);
}

void UPlayerGroundedState::ApplyFriction(const float NormalMagnitude) const
{
	// if applied friction is larger than velocity, velocity is set to zero
	if(BaseState->Velocity.Size() < NormalMagnitude * StaticFrictionCoefficient)
		BaseState->Velocity = FVector::ZeroVector; 
	else
		BaseState->Velocity -= BaseState->Velocity.GetSafeNormal() * NormalMagnitude * KineticFrictionCoefficient; 
}
