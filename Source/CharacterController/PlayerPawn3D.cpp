// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn3D.h"

#include "PlayerAirborneState.h"
#include "PlayerBaseStateComp.h"
#include "PlayerGroundedState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerPawn3D::APlayerPawn3D()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// const FStaticConstructObjectParameters Params = FStaticConstructObjectParameters(UPlayerBaseState::StaticClass()); 
	BaseState = CreateDefaultSubobject<UPlayerBaseStateComp>("Base State");
	GroundedState = CreateDefaultSubobject<UPlayerGroundedState>("Grounded State");
	AirborneState = CreateDefaultSubobject<UPlayerAirborneState>("Airborne State");
}

// Called when the game starts or when spawned
void APlayerPawn3D::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = AirborneState; 
	
	CurrentState->Enter(this);
}

// Called every frame
void APlayerPawn3D::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentState->Update(DeltaTime);
}

// Called to bind functionality to input
void APlayerPawn3D::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CurrentState->SetUpInput(PlayerInputComponent);
}

void APlayerPawn3D::SwitchState(UPlayerBaseStateComp* NewState)
{
	CurrentState->Exit();
	CurrentState = NewState;
	CurrentState->Enter();
}
