// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAirborneState.h"

#include "PlayerGroundedState.h"
#include "PlayerPawn3D.h"
#include "PlayerBaseStateComp.h"

void UPlayerAirborneState::Enter(APlayerPawn3D* PlayerPtr)
{
	PlayerPawnState::Enter(PlayerPtr);
	BaseState = Player->FindComponentByClass<UPlayerBaseStateComp>();
	BaseState->Enter(PlayerPtr);

	// removes input for jump 
	Player->InputComponent->RemoveActionBinding("Jump", IE_Pressed);
}

void UPlayerAirborneState::Update(const float DeltaTime)
{
	BaseState->Update(DeltaTime);

	if(BaseState->bIsGrounded)
		Player->SwitchState(Player->GroundedState);

	UE_LOG(LogTemp, Warning, TEXT("Airborne"))
}
