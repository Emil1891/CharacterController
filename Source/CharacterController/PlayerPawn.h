// Fill out your copyright notice in the Description page of Project Settings.

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
	float MovementSpeed = 400.f;

	FVector CurrentInput = FVector::Zero();

	bool bJump = false;

	UPROPERTY(EditAnywhere)
	float SkinWidth = 1.f;

	UPROPERTY(EditAnywhere)
	float Gravity = 982.f; // not acceleration atm 

	UPROPERTY(EditAnywhere)
	float GroundCheckDistance = 5.f;

	UPROPERTY(EditAnywhere)
	float JumpDistance = 200.f;

	FVector Velocity = FVector::Zero(); 

	// functions
	
	FVector GetLegalMovement(FVector Movement) const;
	
	void MoveSideways(float DeltaTime);

	void ApplyGravity(const float DeltaTime);

	void Jump();

	bool DoLineTrace(FHitResult& HitResultOut, FVector EndLocation) const; 
};
