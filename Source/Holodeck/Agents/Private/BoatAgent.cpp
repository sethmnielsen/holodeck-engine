// Fill out your copyright notice in the Description page of Project Settings.

#include "Holodeck.h"
#include "BoatAgent.h"


// Sets default values
ABoatAgent::ABoatAgent() {
	PrimaryActorTick.bCanEverTick = true;

	// Set the defualt controller
	AIControllerClass = LoadClass<AController>(NULL, TEXT("/Script/Holodeck.BoatAgentController"), NULL, LOAD_None, NULL);
	AutoPossessAI = EAutoPossessAI::PlacedInWorld;
}

void ABoatAgent::InitializeAgent() {
	Super::InitializeAgent();
}

// Called every frame
void ABoatAgent::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	Rotation = CommandArray[0];
	Speed = CommandArray[1];
}

