// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseStateComp.h"
#include "PlayerAirborneState.generated.h"

/**
 * 
 */
UCLASS()
class CHARACTERCONTROLLER_API UPlayerAirborneState : public UPlayerBaseStateComp
{
	GENERATED_BODY()

	virtual void Enter(APlayerPawn3D* PlayerPtr) override;
	virtual void Update(const float DeltaTime) override;
};
