// Fill out your copyright notice in the Description page of Project Settings.

// This class is for 2D movement and was expanded upon for the 3D controller which is more developed/better. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class CHARACTERCONTROLLER_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void JumpInput();
	void HorizontalInput(float Value);
	void VerticalInput(float AxisValue);

private:
    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* PlayerCamera;

	// variables
	
	UPROPERTY(EditAnywhere)
	float Acceleration = 400.f;

	UPROPERTY(EditAnywhere)
	float Deceleration = 400.f;

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1200.f; 

	UPROPERTY(EditAnywhere)
	float SkinWidth = 1.f;

	UPROPERTY(EditAnywhere)
	float Gravity = 982.f; // not acceleration atm 

	UPROPERTY(EditAnywhere)
	float GroundCheckDistance = 5.f;

	UPROPERTY(EditAnywhere)
	float JumpDistance = 200.f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float StaticFrictionCoefficient = 0.5f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float KineticFrictionCoefficient = 0.3f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float AirResistance = 0.2f; 
	
	FVector Input = FVector::Zero();

	bool bJump = false;
	
	FVector Velocity = FVector::Zero(); 

	// functions
	
	void PreventCollision();
	
	void MoveSideways(const float DeltaTime);

	void ApplyGravity(const float DeltaTime);

	void Jump();

	bool DoLineTrace(FHitResult& HitResultOut, FVector EndLocation) const;

	void ApplyFriction(const float NormalMagnitude); 
};
