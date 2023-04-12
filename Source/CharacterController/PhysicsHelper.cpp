// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsHelper.h"

PhysicsHelper::PhysicsHelper()
{
}

FVector PhysicsHelper::GetNormal(FVector Velocity, FVector Normal)
{
	float DotProduct = FVector::DotProduct(Velocity, Normal); 
	if(DotProduct > 0) // if velocity points in the same direction as Normal 
		DotProduct = 0;
	
	const FVector Projection = DotProduct * Normal;
	return -Projection;
}

PhysicsHelper::~PhysicsHelper()
{
}
