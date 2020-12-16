// MIT License (c) 2019 BYU PCCL see LICENSE file

#include "Holodeck.h"
#include "CustomAgentController.h"

ACustomAgentController::ACustomAgentController(const FObjectInitializer& ObjectInitializer)
	: AHolodeckPawnController(ObjectInitializer) {
	UE_LOG(LogTemp, Warning, TEXT("CustomAgent Controller Initialized"));
}

ACustomAgentController::~ACustomAgentController() {}
