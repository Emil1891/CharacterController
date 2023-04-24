// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawnState.h"

PlayerPawnState::PlayerPawnState()
{
}

void PlayerPawnState::Enter(APlayerPawn3D* PlayerPtr)
{
	if(PlayerPtr)
		Player = PlayerPtr; 
}

PlayerPawnState::~PlayerPawnState()
{
}
