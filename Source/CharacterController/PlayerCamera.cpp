// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCamera.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UPlayerCamera::UPlayerCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

// Called when the game starts
void UPlayerCamera::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UPlayerCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RotateCamera();
}

void UPlayerCamera::SetUpCamera(UInputComponent* Input, AActor* PlayerToView)
{
	Player = PlayerToView; 
	Camera = Player->FindComponentByClass<UCameraComponent>();
	StartCameraDistanceFromPlayer = Camera->GetRelativeLocation().X;
	
	if(FirstPersonCamera)
	{
		FVector CameraPos = Camera->GetRelativeLocation();
		CameraPos.X = 0; 
		Camera->SetRelativeLocation(CameraPos);
	}
	
	Input->BindAxis("CameraYaw", this, &UPlayerCamera::CameraYawInput);
	Input->BindAxis("CameraPitch", this, &UPlayerCamera::CameraPitchInput);
}

void UPlayerCamera::CameraYawInput(const float Value)
{
	CameraInput.X += Value * MouseSensitivity; 
}

void UPlayerCamera::CameraPitchInput(const float Value)
{
	if(FirstPersonCamera) 
		CameraInput.Y = std::clamp(CameraInput.Y += Value * MouseSensitivity, MinCameraAngle, MaxCameraAngle);
	else // Third person camera is inversed in a sense, pulling up makes the camera look down instead of up 
		CameraInput.Y = std::clamp(CameraInput.Y += Value * MouseSensitivity, -MaxCameraAngle, -MinCameraAngle);
}

void UPlayerCamera::RotateCamera()
{
	if(FirstPersonCamera)
	{
		// rotate player towards camera horizontally 
		FRotator PlayerNewRot = Player->GetActorRotation();
		PlayerNewRot.Yaw = CameraInput.X;
		Player->SetActorRotation(PlayerNewRot);

		// rotate camera (not player) vertically 
		FRotator CameraNewRot = Camera->GetComponentRotation();
		CameraNewRot.Pitch = CameraInput.Y;
		Camera->SetWorldRotation(CameraNewRot);
	} else // third person camera 
	{
		// sets camera's parent's rotation which is located at the player
		// which makes the camera rotate correctly around the player 
		FRotator CameraNewRot = GetRelativeRotation();
		CameraNewRot.Pitch = CameraInput.Y;
		CameraNewRot.Yaw = CameraInput.X; 
		SetRelativeRotation(CameraNewRot);
		
		// check if the rotation placed the camera behind obstacles
		MoveCameraFromCollision(); 
	}
}

void UPlayerCamera::MoveCameraFromCollision() const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Player);

	// line trace towards camera from the player/camera's parent 
	FHitResult HitResult; 
	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetComponentLocation(),
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
	FVector NewCameraPos = GetComponentTransform().InverseTransformPosition(HitResult.ImpactPoint); 
		
	Camera->SetRelativeLocation(FVector(NewCameraPos.X, 0, 0));
}