// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn3D.h"

#include "FrameTypes.h"
#include "PhysicsHelper.h"
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

	Camera = FindComponentByClass<UCameraComponent>();
	StartCameraDistanceFromPlayer = Camera->GetRelativeLocation().X; 
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

	RotateCamera();
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
		GetWorld()->OverlapMultiByChannel(Overlapping, Origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeCapsule(Extent), QueryParams);

		if(Overlapping.IsEmpty()) // no overlaps 
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

void APlayerPawn3D::CameraYawInput(const float Value)
{
	CameraInput.X += Value * MouseSensitivity; 
}

void APlayerPawn3D::CameraPitchInput(const float Value)
{
	if(FirstPersonCamera) 
		CameraInput.Y = std::clamp(CameraInput.Y += Value * MouseSensitivity, MinCameraAngle, MaxCameraAngle);
	else // Third person camera is inversed in a sense, pulling up makes the camera look down instead of up 
		CameraInput.Y = std::clamp(CameraInput.Y += Value * MouseSensitivity, -MaxCameraAngle, -MinCameraAngle);
}

void APlayerPawn3D::RotateCamera()
{
	if(FirstPersonCamera)
	{
		// rotate player towards camera horizontally 
		FRotator PlayerNewRot = GetActorRotation();
		PlayerNewRot.Yaw = CameraInput.X;
		SetActorRotation(PlayerNewRot);

		// rotate camera (not player) vertically 
		FRotator CameraNewRot = Camera->GetComponentRotation();
		CameraNewRot.Pitch = CameraInput.Y;
		Camera->SetWorldRotation(CameraNewRot);
	} else // third person camera 
	{
		USceneComponent* CameraParent = Camera->GetAttachParent();

		// sets camera's parent's rotation which is located at the player
		// which makes the camera rotate correctly around the player 
		FRotator CameraNewRot = CameraParent->GetRelativeRotation();
		CameraNewRot.Pitch = CameraInput.Y;
		CameraNewRot.Yaw = CameraInput.X; 
		CameraParent->SetRelativeRotation(CameraNewRot);
		
		// check if the rotation placed the camera behind obstacles
		MoveCameraFromCollision(CameraParent); 
	}
}

void APlayerPawn3D::MoveCameraFromCollision(USceneComponent* CameraParent) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// line trace towards camera from the player/camera's parent 
	FHitResult HitResult; 
	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		CameraParent->GetComponentLocation(),
		Camera->GetComponentLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(CameraLineTraceRadius), 
		QueryParams);

	if(!bHit)
	{
		// if not hit, interpolate camera back to original distance 
		const double CurrentX = Camera->GetRelativeLocation().X; 
		const double NewX = UKismetMathLibrary::Lerp(CurrentX, StartCameraDistanceFromPlayer, CameraMoveBackSpeed); 
		Camera->SetRelativeLocation(FVector(NewX, 0, 0));
		return;
	}

	// gets the local position of the impact point, in reference to the camera's parent 
	FVector NewCameraPos = CameraParent->GetComponentTransform().InverseTransformPosition(HitResult.ImpactPoint); 
		
	Camera->SetRelativeLocation(FVector(NewCameraPos.X, 0, 0));
}

void APlayerPawn3D::MoveSideways(const float DeltaTime)
{
	const double Distance = Acceleration * DeltaTime;
	const FVector Movement = Input.GetSafeNormal() * Distance;
	Velocity += Movement; 
}

void APlayerPawn3D::ApplyGravity(const float DeltaTime)
{
	const FVector Movement = FVector::DownVector * Gravity * DeltaTime;
	Velocity += Movement;
}

void APlayerPawn3D::Jump()
{
	if(CheckGrounded().IsValidBlockingHit()) // now the player just teleports
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

	// camera 
	PlayerInputComponent->BindAxis("CameraYaw", this, &APlayerPawn3D::CameraYawInput);
	PlayerInputComponent->BindAxis("CameraPitch", this, &APlayerPawn3D::CameraPitchInput);
}

void APlayerPawn3D::JumpInput()
{
	bJump = true; 
}

void APlayerPawn3D::HorizontalInput(float AxisValue)
{
	// resets input 
	Input = FVector::Zero();
	
	FVector NewInput = AxisValue * Camera->GetRightVector();
	const FVector GroundNormal = CheckGrounded().ImpactNormal; 
	NewInput = FVector::VectorPlaneProject(NewInput, GroundNormal);
	Input += NewInput; 
}

void APlayerPawn3D::VerticalInput(float AxisValue)
{
	FVector NewInput = AxisValue * Camera->GetForwardVector();
	const FVector GroundNormal = CheckGrounded().ImpactNormal; 
	NewInput = FVector::VectorPlaneProject(NewInput, GroundNormal);
	Input += NewInput; 
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
		FCollisionShape::MakeBox(Extent), 
		QueryParams);
}
