// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn3D.h"

#include "PhysicsHelper.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerPawn3D::APlayerPawn3D()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	// PlayerCamera->SetupAttachment(RootComponent);
	// PlayerCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	// PlayerCamera->SetRelativeLocation(FVector(0.0f, 1600.0f, 200.0f));
	// PlayerCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	// PlayerCamera->OrthoWidth = 8000.f;

}

// Called when the game starts or when spawned
void APlayerPawn3D::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ApplyGravity(DeltaTime);
	
	MoveSideways(DeltaTime);

	// applies air resistance 
	Velocity *= FMath::Pow(AirResistance, DeltaTime);

	if(bJump)
		Jump();

	PreventCollision(); 

	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
}

void APlayerPawn3D::MoveSideways(const float DeltaTime)
{
	const double Distance = Acceleration * DeltaTime;
	const FVector Movement = Input.GetSafeNormal() * Distance;
	Velocity += Movement; 
}

void APlayerPawn3D::ApplyGravity(const float DeltaTime)
{
	const FVector MoveDistance = FVector::DownVector * Gravity * DeltaTime;
	Velocity += MoveDistance;
}

void APlayerPawn3D::Jump()
{
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);

	FHitResult HitResult;
	const bool bGrounded = DoLineTrace(HitResult, Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth));

	if(bGrounded) // now the player just teleports
		Velocity += FVector::UpVector * JumpDistance; 

	bJump = false; 
}

// Called to bind functionality to input
void APlayerPawn3D::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction(
	   "Jump",
	   IE_Pressed,
	   this,
	   &APlayerPawn3D::JumpInput);

	InputComponent->BindAxis(
		"Horizontal", 
		this, 
		&APlayerPawn3D::HorizontalInput);
	InputComponent->BindAxis(
		"Vertical", 
		this, 
		&APlayerPawn3D::VerticalInput);

}

void APlayerPawn3D::JumpInput()
{
	bJump = true; 
}

void APlayerPawn3D::HorizontalInput(float AxisValue)
{
	Input.X = AxisValue;
}

void APlayerPawn3D::VerticalInput(float AxisValue)
{
	Input.Y = -AxisValue; // negative because it is otherwise inversed  
}

void APlayerPawn3D::PreventCollision() 
{
	constexpr float TooSmallMovement = 0.001f;
	constexpr int MaxLoops = 10;
	int Loops = 0; 
	bool bHit = false;
	const float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

	// iterative section to prevent collisions occuring after the previous adjustment 
	while(Loops++ < MaxLoops)
	{
		// If movement is too small to notice, velocity is set to zero 
		if(Velocity.Size() * DeltaTime < TooSmallMovement)
		{
			Velocity = FVector::Zero(); 
			return;
		}

		FVector Movement = Velocity * DeltaTime; 

		// Line trace 
		FVector Origin, Extent;
		GetActorBounds(true, Origin, Extent);

		FHitResult HitResult;
		bHit = DoLineTrace(HitResult, Origin + Movement.GetSafeNormal() * (Movement.Size() + SkinWidth));
		// Line trace end
		
		if(!bHit) // no collision -> no adjustment necessary 
			return; 
		
		const FVector Normal = PhysicsHelper::GetNormal(Velocity, HitResult.Normal);
		Velocity += Normal;

		ApplyFriction(Normal.Size()); 
	}
}

void APlayerPawn3D::ApplyFriction(const float NormalMagnitude)
{
	// if applied friction is larger than velocity, velocity is set to zero
	if(Velocity.Size() < NormalMagnitude * StaticFrictionCoefficient)
		Velocity = FVector::ZeroVector; 
	else
		Velocity -= Velocity.GetSafeNormal() * NormalMagnitude * KineticFrictionCoefficient; 
}

bool APlayerPawn3D::DoLineTrace(FHitResult& HitResultOut, FVector EndLocation) const
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
		FCollisionShape::MakeCapsule(Extent),
		QueryParams);
}
