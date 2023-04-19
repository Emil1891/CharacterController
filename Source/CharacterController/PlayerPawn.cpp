// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"

#include "PhysicsHelper.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
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
	
	ApplyGravity(DeltaTime);
	
	MoveSideways(DeltaTime);

	// applies air resistance 
	Velocity *= FMath::Pow(AirResistance, DeltaTime);

	if(bJump)
		Jump();

	PreventCollision(); 

	SetActorLocation(GetActorLocation() + Velocity * DeltaTime); 
}

void APlayerPawn::MoveSideways(const float DeltaTime)
{
	const double Distance = Acceleration * DeltaTime;
	const FVector Movement = Input.GetSafeNormal() * Distance; // Input.GetSafeNormal() is the direction 
	Velocity += Movement; 
}

void APlayerPawn::ApplyGravity(const float DeltaTime)
{
	const FVector Movement = FVector::DownVector * Gravity * DeltaTime;
	Velocity += Movement;
}

void APlayerPawn::Jump()
{
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);

	FHitResult HitResult;
	const bool bGrounded = DoLineTrace(HitResult, Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth));

	if(bGrounded) 
		Velocity += FVector::UpVector * JumpDistance; 

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
	Input.X = AxisValue;
}

void APlayerPawn::VerticalInput(float AxisValue)
{
	// not used as of now 
	// CurrentInput.Z = AxisValue; 
}

void APlayerPawn::PreventCollision() 
{
	constexpr float TooSmallMovement = 0.001f;
	constexpr int MaxLoops = 10;
	int Loops = 0; 
	bool bHit = false;
	const float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

	// iterative section to prevent collisions occuring after the previous adjustment
	// i.e collisions occuring because of the adjusted movement after the first collision 
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

		/* TODO: Attempts start */
		
		// sets location to where the player would end up at collision, skin width adjusted

		/* First attempt */
		// FVector HitDirection = HitResult.Location - Origin;
		// SetActorLocation(HitResult.Location - HitDirection.GetSafeNormal() * SkinWidth);

		/* Second attempt - Best attempt */
		SetActorLocation(GetActorLocation() - HitResult.Normal * (HitResult.Distance - SkinWidth));

		/* Third attempt */
		// double DistanceToColliderNeg = SkinWidth / FVector::DotProduct(Movement.GetSafeNormal(),HitResult.Normal);
		// double AllowedMovementDistance = HitResult.Distance + DistanceToColliderNeg;
		// if (AllowedMovementDistance > Movement.Size())
		// 	return; 
		// 	// return Movement;
		// if (AllowedMovementDistance > 0.0f)
		// 	SetActorLocation(GetActorLocation() + Movement.GetSafeNormal() * AllowedMovementDistance);

		/* Attempts over */
		
		ApplyFriction(Normal.Size()); 
	}
}

void APlayerPawn::ApplyFriction(const float NormalMagnitude)
{
	// if applied friction is larger than velocity, velocity is set to zero
	if(Velocity.Size() < NormalMagnitude * StaticFrictionCoefficient)
		Velocity = FVector::ZeroVector; 
	else
		Velocity -= Velocity.GetSafeNormal() * NormalMagnitude * KineticFrictionCoefficient; 
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
