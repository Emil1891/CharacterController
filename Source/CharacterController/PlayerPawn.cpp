// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"

#include "PhysicsHelper.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
 //     
	// PlayerCamera->SetupAttachment(RootComponent);
	// PlayerCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	// PlayerCamera->SetRelativeLocation(FVector(0.0f, 1600.0f, 200.0f));
	// PlayerCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
 //    
	// PlayerCamera->OrthoWidth = 8000.f;

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
	
	Velocity = FVector::Zero();
	
	MoveSideways(DeltaTime);
	ApplyGravity(DeltaTime);

	if(bJump)
		Jump();

	SetActorLocation(GetActorLocation() + GetLegalMovement(Velocity)); 
}

void APlayerPawn::MoveSideways(float DeltaTime)
{
	const FVector CurrentLocation = GetActorLocation();
	const double Distance = MovementSpeed * DeltaTime;
	const FVector Movement = CurrentInput.GetSafeNormal() * Distance;
	// SetActorLocation(CurrentLocation + GetLegalMovement(Movement));
	Velocity += Movement; 
}

void APlayerPawn::ApplyGravity(const float DeltaTime)
{
	const FVector MoveDistance = FVector::DownVector * Gravity * DeltaTime;
	// SetActorLocation(GetActorLocation() + GetLegalMovement(MoveDistance));
	Velocity += MoveDistance;
}

void APlayerPawn::Jump()
{
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);

	FHitResult HitResult;
	const bool bHit = DoLineTrace(HitResult, Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth));

	if(bHit) // now the player just teleports
		Velocity += FVector::UpVector * JumpDistance; 
		// SetActorLocation(GetActorLocation() + FVector::UpVector * JumpDistance);

	bJump = false; 
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
	CurrentInput.X = AxisValue;
}

void APlayerPawn::VerticalInput(float AxisValue)
{
	// CurrentInput.Z = AxisValue; 
}

FVector APlayerPawn::GetLegalMovement(FVector Movement) const
{
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);

	FHitResult HitResult;
	const bool bHit = DoLineTrace(HitResult, Origin + Movement.GetSafeNormal() * (Movement.Size() + SkinWidth));
	
	if(!bHit) // no hit, you can move the full extent of movement
		return Movement;
	
	// if hit, return distance to hit point plus normal 
	return Movement.GetSafeNormal() * (HitResult.Distance - SkinWidth) + PhysicsHelper::GetNormal(Velocity, HitResult.Normal);
}

bool APlayerPawn::DoLineTrace(FHitResult& HitResultOut, FVector EndLocation) const
{
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	return GetWorld()->SweepSingleByChannel(
		HitResultOut,
		Origin,
		EndLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeBox(Extent),
		QueryParams);
}