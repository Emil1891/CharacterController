// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerPawnState.h"
#include "Components/ActorComponent.h"
#include "PlayerBaseStateComp.generated.h"

/**
 * This state holds all functionality that is shared between all other states  
 */

class APlayerPawn3D; 

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHARACTERCONTROLLER_API UPlayerBaseStateComp : public UActorComponent, public PlayerPawnState
{
	GENERATED_BODY()

public:
	UPlayerBaseStateComp() = default;

	virtual void Enter(APlayerPawn3D* PlayerPtr = nullptr) override;
	virtual void Update(const float DeltaTime) override;
	virtual void Exit() override {};

	void SetUpInput(); 
	
	void HorizontalInput(const float AxisValue);
	void VerticalInput(const float AxisValue);

	~UPlayerBaseStateComp() = default;

private:	
	// variables
	bool bIsGrounded = false;
	
	FVector Velocity = FVector::Zero(); 

	UPROPERTY()
	class UPlayerCamera* CameraComp;
	
	UPROPERTY(EditAnywhere)
	float Acceleration = 400.f;

	UPROPERTY(EditAnywhere)
	float Deceleration = 400.f;

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1200.f; 

	UPROPERTY(EditAnywhere)
	float SkinWidth = 1.f; 

	UPROPERTY(EditAnywhere)
	float Gravity = 982.f;  

	UPROPERTY(EditAnywhere)
	float GroundCheckDistance = 5.f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float AirResistance = 0.2f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f, ClampMax = 90.f, UIMin = 0.f, UIMax = 90.f))
	float MaxSlopeAngle = 60.f; 
	
	FVector Input = FVector::Zero();

	bool bHasBeenSetUp = false;

	// functions
	void PreventCollision();
	
	void MoveSideways(const float DeltaTime);

	void ApplyGravity(const float DeltaTime);

	bool DoLineTrace(FHitResult& HitResultOut, const FVector& EndLocation) const;

	void AdjustForOverlap();

	FHitResult CheckGrounded();

	// friends 
	friend class UPlayerGroundedState;
	friend class UPlayerAirborneState;
};
