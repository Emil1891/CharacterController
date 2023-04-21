// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn3D.h"

#include "FrameTypes.h"
#include "PhysicsHelper.h"
#include "PlayerCamera.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerPawn3D::APlayerPawn3D()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void APlayerPawn3D::BeginPlay()
{
	Super::BeginPlay();

	CameraComp = FindComponentByClass<UPlayerCamera>();
	
	CameraComp->SetPlayer(this);
	// set up is called here instead of in SetUpPlayerInput because of the call order:
	// SetUpInput is called before begin play making the Camera uninitialized 
	CameraComp->SetUpCamera(InputComponent);
}

// Called every frame
void APlayerPawn3D::Tick(const float DeltaTime)
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

	AdjustForOverlap();
}

void APlayerPawn3D::AdjustForOverlap()
{
	int Loops = 0;
	constexpr int MaxLoops = 15;
	// this loop will run until there are no overlaps or until it has reached max loops
	while(Loops++ < MaxLoops)
	{
		FVector Origin, Extent;
		GetActorBounds(true, Origin, Extent);
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		TArray<FOverlapResult> Overlapping;

		// Overlaps are rechecked after each time the player is moved 
		const bool HasOverlap = GetWorld()->OverlapMultiByChannel(Overlapping, Origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeCapsule(Extent), QueryParams);

		if(!HasOverlap) // no overlaps 
			return;
		
		FMTDResult MTD;
		const bool OverlapExists = Overlapping[0].GetComponent()->ComputePenetration(MTD, FCollisionShape::MakeCapsule(Extent), Origin, FQuat::Identity);

		if(!OverlapExists)
			continue;

		// Move player so there is no overlap 
		SetActorLocation(GetActorLocation() + MTD.Direction * (MTD.Distance + SkinWidth));
		Velocity += PhysicsHelper::GetNormal(Velocity, -MTD.Direction); 
	}
}

void APlayerPawn3D::MoveSideways(const float DeltaTime)
{
	const double Distance = Acceleration * DeltaTime;
	const FVector Movement = Input.GetSafeNormal() * Distance; // Input.GetSafeNormal() is the movement direction 
	Velocity += Movement; 
}

void APlayerPawn3D::ApplyGravity(const float DeltaTime)
{
	const FVector Movement = FVector::DownVector * Gravity * DeltaTime;
	Velocity += Movement;
}

void APlayerPawn3D::Jump()
{
	if(CheckGrounded().IsValidBlockingHit()) // if ground check hits ground 
		Velocity += FVector::UpVector * JumpDistance; 

	bJump = false; 
}

FHitResult APlayerPawn3D::CheckGrounded() const
{
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);

	FHitResult HitResult;
	DoLineTrace(HitResult, Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth));

	return HitResult; 
}

// Called to bind functionality to input
void APlayerPawn3D::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent = PlayerInputComponent; 

	// jump
	PlayerInputComponent->BindAction(
	   "Jump",
	   IE_Pressed,
	   this,
	   &APlayerPawn3D::JumpInput);

	// walk
	PlayerInputComponent->BindAxis(
		"Horizontal", 
		this, 
		&APlayerPawn3D::HorizontalInput);
	
	PlayerInputComponent->BindAxis(
		"Vertical", 
		this, 
		&APlayerPawn3D::VerticalInput);
}

void APlayerPawn3D::JumpInput()
{
	bJump = true; 
}

// helper function to get angle between 2 vectors 
static float GetAngle(const FVector& V1, const FVector& V2)
{
	const float Dot = FVector::DotProduct(V1.GetSafeNormal(), V2.GetSafeNormal());
	// UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), FMath::RadiansToDegrees(FMath::Acos(Dot)) - 90)
	return FMath::RadiansToDegrees(FMath::Acos(Dot)) - 90; 
}

// TODO: angle thing does not work when walking diagonally 
void APlayerPawn3D::HorizontalInput(const float AxisValue)
{
	// resets input 
	Input = FVector::Zero();

	if(AxisValue == 0)
		return; 
	
	FVector NewInput = AxisValue * CameraComp->GetCamera()->GetRightVector();
	const FHitResult HitResult = CheckGrounded(); 
	const FVector GroundNormal = HitResult.ImpactNormal.GetSafeNormal();

	// gets a vector that is parallel with the horizontal axis, regardless of camera transform 
	const FVector DirectionTowardsSlope = FVector::VectorPlaneProject(NewInput, FVector::UpVector);
	
	// if traversing a too steep slope, simply return without registering input
	if(GetAngle(GroundNormal, DirectionTowardsSlope) > MaxSlopeAngle)
		return;

	NewInput = FVector::VectorPlaneProject(NewInput, GroundNormal);
	Input += NewInput; 
}

void APlayerPawn3D::VerticalInput(const float AxisValue)
{
	if(AxisValue == 0)
		return; 
	
	FVector NewInput = AxisValue * CameraComp->GetCamera()->GetForwardVector();
	const FHitResult HitResult = CheckGrounded(); 
	const FVector GroundNormal = HitResult.ImpactNormal.GetSafeNormal();

	// gets a vector that is parallel with the horizontal axis, regardless of camera transform 
	const FVector DirectionTowardsSlope = FVector::VectorPlaneProject(NewInput, FVector::UpVector);
	
	// if traversing a too steep slope, simply return without registering input
	if(GetAngle(GroundNormal, DirectionTowardsSlope) > MaxSlopeAngle)
		return;

	NewInput = FVector::VectorPlaneProject(NewInput, GroundNormal);
	Input += NewInput; 
}

void APlayerPawn3D::PreventCollision() 
{
	constexpr float TooSmallMovement = 0.001f;
	constexpr int MaxLoops = 10;
	int Loops = 0; 
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
		const bool bHit = DoLineTrace(HitResult, Origin + Movement.GetSafeNormal() * (Movement.Size() + SkinWidth));
		// Line trace end
		
		if(!bHit) // no collision -> no adjustment necessary 
			return;
		
		const FVector Normal = PhysicsHelper::GetNormal(Velocity, HitResult.Normal);
		Velocity += Normal;
		
		// FVector HitDirection = HitResult.Location - Origin;
		// // sets location to where the player would end up at collision, skin width adjusted 
		// SetActorLocation(HitResult.Location - HitDirection.GetSafeNormal() * SkinWidth);
		
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

bool APlayerPawn3D::DoLineTrace(FHitResult& HitResultOut, const FVector& EndLocation) const
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
