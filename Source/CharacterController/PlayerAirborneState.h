// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerPawnState.h"
#include "Components/ActorComponent.h"
#include "PlayerAirborneState.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHARACTERCONTROLLER_API UPlayerAirborneState : public UActorComponent, public PlayerPawnState
{
	GENERATED_BODY()

public:
	virtual void Enter(APlayerPawn3D* PlayerPtr) override;
	virtual void Update(const float DeltaTime) override;

private:
	UPROPERTY()
	class UPlayerBaseStateComp* BaseState;
};
