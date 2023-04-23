// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAirborneState.h"

#include "PlayerGroundedState.h"
#include "PlayerPawn3D.h"

void UPlayerAirborneState::Enter(APlayerPawn3D* PlayerPtr)
{
	Super::Enter(PlayerPtr);

	// removes input for jump 
	InputComp->RemoveActionBinding("Jump", IE_Pressed);
}

void UPlayerAirborneState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	if(bIsGrounded)
		Player->SwitchState(Player->GroundedState);
}
