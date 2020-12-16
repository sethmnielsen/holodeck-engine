// MIT License (c) 2019 BYU PCCL see LICENSE file

#pragma once

#include "Holodeck.h"

#include "HolodeckPawnController.h"
#include "CustomAgent.h"

#include "CustomAgentController.generated.h"

/**
* A Holodeck Custom Agent Controller
*/
UCLASS()
class HOLODECK_API ACustomAgentController : public AHolodeckPawnController
{
	GENERATED_BODY()

public:
	/**
	* Default Constructor
	*/
	ACustomAgentController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* Default Destructor
	*/
	~ACustomAgentController();

	void AddControlSchemes() override {
		// No control schemes
	}
};
