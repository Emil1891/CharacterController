// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "Camera/CameraComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
     
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->SetRelativeLocation(FVector(0.0f, 1600.0f, 200.0f));
	PlayerCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    
	PlayerCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	PlayerCamera->OrthoWidth = 3200.f;

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector CurrentLocation = GetActorLocation();
	const double Distance = MovementSpeed * DeltaTime;
	SetActorLocation(CurrentLocation + CurrentInput * Distance);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction(
	   "Jump",
	   IE_Pressed,
	   this,
	   &APlayerPawn::JumpInput);

	InputComponent->BindAxis(
		"Horizontal", 
		this, 
		&APlayerPawn::HorizontalInput);
	InputComponent->BindAxis(
		"Vertical", 
		this, 
		&APlayerPawn::VerticalInput);

}

void APlayerPawn::JumpInput()
{
	bJump = true; 
}

void APlayerPawn::HorizontalInput(float AxisValue)
{
	CurrentInput = FVector(AxisValue, 0.0f, 0.0f);
}

void APlayerPawn::VerticalInput(float Value)
{
	
}

