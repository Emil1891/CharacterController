// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBaseStateComp.h"
#include "PhysicsHelper.h"
#include "PlayerCamera.h"
#include "PlayerGroundedState.h"
#include "PlayerPawn3D.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

UPlayerBaseStateComp::UPlayerBaseStateComp()
{
}

void UPlayerBaseStateComp::Enter(APlayerPawn3D* PlayerPtr)
{
	if(PlayerPtr)
		Player = PlayerPtr;
	
	CameraComp = Player->FindComponentByClass<UPlayerCamera>();
	
	// set up is called here instead of in SetUpPlayerInput because of the call order:
	// SetUpInput is called before begin play making the Camera uninitialized 
	CameraComp->SetUpCamera(InputComp, Player);
}

void UPlayerBaseStateComp::SetUpInput(UInputComponent* PlayerInputComponent)
{
	InputComp = PlayerInputComponent;

	// walk
	PlayerInputComponent->BindAxis(
		"Horizontal", 
		this, 
		&UPlayerBaseStateComp::HorizontalInput);
	
	PlayerInputComponent->BindAxis(
		"Vertical", 
		this, 
		&UPlayerBaseStateComp::VerticalInput);
}

void UPlayerBaseStateComp::Update(const float DeltaTime)
{
	ApplyGravity(DeltaTime);
	
	MoveSideways(DeltaTime);

	// applies air resistance 
	Velocity *= FMath::Pow(AirResistance, DeltaTime); 

	PreventCollision();
	
	Player->SetActorLocation(Player->GetActorLocation() + Velocity * DeltaTime);

	AdjustForOverlap();
}

void UPlayerBaseStateComp::AdjustForOverlap()
{
	int Loops = 0;
	constexpr int MaxLoops = 15;
	// this loop will run until there are no overlaps or until it has reached max loops
	while(Loops++ < MaxLoops)
	{
		FVector Origin, Extent;
		Player->GetActorBounds(true, Origin, Extent);
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Player);
		TArray<FOverlapResult> Overlapping;

		// Overlaps are rechecked after each time the player is moved 
		const bool HasOverlap = Player->GetWorld()->OverlapMultiByChannel(Overlapping, Origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeCapsule(Extent), QueryParams);

		if(!HasOverlap) // no overlaps 
			return;
		
		FMTDResult MTD;
		const bool OverlapExists = Overlapping[0].GetComponent()->ComputePenetration(MTD, FCollisionShape::MakeCapsule(Extent), Origin, FQuat::Identity);

		if(!OverlapExists)
			continue;

		// Move player so there is no overlap 
		Player->SetActorLocation(Player->GetActorLocation() + MTD.Direction * (MTD.Distance + SkinWidth));
		Velocity += PhysicsHelper::GetNormal(Velocity, -MTD.Direction); 
	}
}

void UPlayerBaseStateComp::MoveSideways(const float DeltaTime)
{
	const double Distance = Acceleration * DeltaTime;
	const FVector Movement = Input.GetSafeNormal() * Distance; // Input.GetSafeNormal() is the movement direction 
	Velocity += Movement; 
}

void UPlayerBaseStateComp::ApplyGravity(const float DeltaTime)
{
	const FVector Movement = FVector::DownVector * Gravity * DeltaTime;
	Velocity += Movement;
}

FHitResult UPlayerBaseStateComp::CheckGrounded() const
{
	FVector Origin, Extent;
	Player->GetActorBounds(true, Origin, Extent);

	FHitResult HitResult;
	DoLineTrace(HitResult, Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth));

	return HitResult; 
}

// helper function to get angle between 2 vectors 
static float GetAngle(const FVector& V1, const FVector& V2)
{
	const float Dot = FVector::DotProduct(V1.GetSafeNormal(), V2.GetSafeNormal());
	// UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), FMath::RadiansToDegrees(FMath::Acos(Dot)) - 90)
	return FMath::RadiansToDegrees(FMath::Acos(Dot)) - 90; 
}

// TODO: angle thing does not work when walking diagonally 
void UPlayerBaseStateComp::HorizontalInput(const float AxisValue)
{
	// resets input 
	Input = FVector::Zero();

	// early return if no input so to prevent unnecessary calculations below 
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

void UPlayerBaseStateComp::VerticalInput(const float AxisValue)
{
	// early return if no input so to prevent unnecessary calculations below 
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

void UPlayerBaseStateComp::PreventCollision() 
{
	constexpr float TooSmallMovement = 0.001f;
	constexpr int MaxLoops = 10;
	int Loops = 0; 
	const float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(Player);

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
		Player->GetActorBounds(true, Origin, Extent);

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

		// if grounded TODO: refactor this somehow so it's placed in the grounded state class. the problem is getting the normal in that class 
		if(UPlayerGroundedState* Grounded = dynamic_cast<UPlayerGroundedState*>(Player->CurrentState))
			Grounded->ApplyFriction(Normal.Size()); 
	}
}

bool UPlayerBaseStateComp::DoLineTrace(FHitResult& HitResultOut, const FVector& EndLocation) const
{
	FVector Origin, Extent;
	Player->GetActorBounds(true, Origin, Extent);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Player);

	return Player->GetWorld()->SweepSingleByChannel(
		HitResultOut,
		Origin,
		EndLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(Extent), 
		QueryParams);
}

UPlayerBaseStateComp::~UPlayerBaseStateComp()
{
}
