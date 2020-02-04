// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Holodeck.h"
#include "HolodeckPawnController.h"
#include "BoatAgent.h"

#include "BoatController.generated.h"

/**
 * 
 */
UCLASS()
class HOLODECK_API ABoatController : public AHolodeckPawnController
{
	GENERATED_BODY()
	
public:
	/**
	  * Default Constructor
	  */
	ABoatController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	  * Default Destructor
	  */
	~ABoatController();

	void AddControlSchemes() override {
		// No control schemes
	}
	
	
};
