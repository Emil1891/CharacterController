// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn3D.generated.h"

class PlayerPawnState;

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
	virtual void Tick(const float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void SwitchState(PlayerPawnState* NewState); 

	PlayerPawnState* CurrentState; 
	
	UPROPERTY(EditAnywhere)
	class UPlayerBaseStateComp* BaseState;

	UPROPERTY(EditAnywhere)
	class UPlayerGroundedState* GroundedState;

	UPROPERTY(EditAnywhere)
	class UPlayerAirborneState* AirborneState;

	friend class UPlayerBaseStateComp; 
};
