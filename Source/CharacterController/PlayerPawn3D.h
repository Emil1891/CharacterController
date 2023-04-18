// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn3D.generated.h"

UCLASS()
class CHARACTERCONTROLLER_API APlayerPawn3D : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn3D();

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

	void AdjustForOverlap();

	FHitResult CheckGrounded() const; 

#pragma region Camera
	
    UPROPERTY(VisibleAnywhere, Category = "Player Camera")
    class UCameraComponent* PlayerCamera;
	
	void CameraYawInput(float Value);
	void CameraPitchInput(float Value);
	void RotateCamera();
	void MoveCameraFromCollision(USceneComponent* CameraParent) const; 

	UCameraComponent* Camera; 
	FVector CameraInput = FVector::Zero();

	UPROPERTY(EditAnywhere, Category = "Player Camera")
	float MouseSensitivity = 5.f;

	UPROPERTY(EditAnywhere, Category = "Player Camera")
	double MaxCameraAngle = 70;
	
	UPROPERTY(EditAnywhere, Category = "Player Camera")
	double MinCameraAngle = -70;

	UPROPERTY(EditDefaultsOnly, Category = "Player Camera")
	bool FirstPersonCamera = true;

	UPROPERTY(EditAnywhere, Category = "Player Camera")
	float CameraLineTraceRadius = 5.f; 
	
	float StartCameraDistanceFromPlayer;

	UPROPERTY(EditAnywhere, Category="Player Camera", meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	double CameraMoveBackSpeed = 0.3f; 

#pragma endregion 
};
