// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * State interface-ish 
 */
class CHARACTERCONTROLLER_API PlayerPawnState
{
public:
	PlayerPawnState() {};

	virtual void Enter(class APlayerPawn3D* PlayerPtr = nullptr);
	virtual void Update(const float DeltaTime) {};
	virtual void Exit() {}; 
	
	virtual ~PlayerPawnState() {};

protected:
	APlayerPawn3D* Player;
};
