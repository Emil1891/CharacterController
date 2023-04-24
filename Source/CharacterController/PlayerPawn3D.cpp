// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn3D.h"

#include "PlayerAirborneState.h"
#include "PlayerBaseStateComp.h"
#include "PlayerGroundedState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerPawnState.h"

// Sets default values
APlayerPawn3D::APlayerPawn3D()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseState = CreateDefaultSubobject<UPlayerBaseStateComp>("Base State");
	GroundedState = CreateDefaultSubobject<UPlayerGroundedState>("Grounded State");
	AirborneState = CreateDefaultSubobject<UPlayerAirborneState>("Airborne State");

	CurrentState = GroundedState; 
}

// Called when the game starts or when spawned
void APlayerPawn3D::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentState->Enter(this);
}

// Called every frame
void APlayerPawn3D::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentState->Update(DeltaTime);
}

void APlayerPawn3D::SwitchState(PlayerPawnState* NewState)
{
	CurrentState->Exit();
	CurrentState = NewState;
	CurrentState->Enter(this);
}
