// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerPawnState.h"
#include "Components/ActorComponent.h"
#include "PlayerGroundedState.generated.h"

/**
 * 
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHARACTERCONTROLLER_API UPlayerGroundedState : public UActorComponent, public PlayerPawnState
{
	GENERATED_BODY()
	
public:
	virtual void Enter(APlayerPawn3D* PlayerPtr) override;
	virtual void Update(const float DeltaTime) override;
	
	void JumpInput();
	
	void ApplyFriction(const float NormalMagnitude) const;
	
private:
	UPROPERTY(EditAnywhere)
	float JumpDistance = 200.f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float StaticFrictionCoefficient = 0.5f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float KineticFrictionCoefficient = 0.3f;

	bool bJump = false;

	class UPlayerBaseStateComp* BaseState;

	void Jump();
};
