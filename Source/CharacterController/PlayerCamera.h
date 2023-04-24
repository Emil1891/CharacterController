// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerCamera.generated.h"

class UCameraComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHARACTERCONTROLLER_API UPlayerCamera : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerCamera();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetUpCamera(); 

	UCameraComponent* GetCamera() const { return Camera; }
	
	void CameraYawInput(const float Value);
	void CameraPitchInput(const float Value);
	
private:
	AActor* Player;
	
	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* Camera;
	
	void RotateCamera();
	void MoveCameraFromCollision() const;

	FVector CameraInput = FVector::Zero();

	UPROPERTY(EditAnywhere, Category = "Player Camera")
	float MouseSensitivity = 5.f;

	UPROPERTY(EditAnywhere, Category = "Player Camera")
	double MaxCameraAngle = 70;
	
	UPROPERTY(EditAnywhere, Category = "Player Camera")
	double MinCameraAngle = -70;

	UPROPERTY(EditDefaultsOnly, Category = "Player Camera")
	bool FirstPersonCamera = true;

	UPROPERTY(EditAnywhere, Category = "Player Camera")
	float CameraLineTraceRadius = 5.f; 
	
	float StartCameraDistanceFromPlayer = -1;

	UPROPERTY(EditAnywhere, Category="Player Camera", meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	double CameraMoveBackSpeed = 0.3f; 
		
};
