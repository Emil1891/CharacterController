// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class CHARACTERCONTROLLER_API PhysicsHelper
{
public:
	PhysicsHelper();

	static FVector GetNormal(FVector Velocity, FVector Normal); 
	
	~PhysicsHelper();
};
