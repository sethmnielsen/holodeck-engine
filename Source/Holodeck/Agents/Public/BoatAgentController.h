// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Holodeck.h"
#include "HolodeckPawnController.h"
#include "BoatAgent.h"

#include "BoatAgentController.generated.h"

/**
 *
 */
UCLASS()
class HOLODECK_API ABoatAgentController : public AHolodeckPawnController
{
	GENERATED_BODY()

public:
	/**
	  * Default Constructor
	  */
	ABoatAgentController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	  * Default Destructor
	  */
	~ABoatAgentController();

	void AddControlSchemes() override {
		// No control schemes
	}


};
