// Fill out your copyright notice in the Description page of Project Settings.

#include "Holodeck.h"
#include "BoatAgentController.h"

ABoatAgentController::ABoatAgentController(const FObjectInitializer& ObjectInitializer)
		: AHolodeckPawnController(ObjectInitializer) {
	UE_LOG(LogHolodeck, Log, TEXT("BoatAgent Controller Initialized"));
}

ABoatAgentController::~ABoatAgentController() {}
