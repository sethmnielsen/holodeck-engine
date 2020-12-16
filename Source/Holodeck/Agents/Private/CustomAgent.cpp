// MIT License (c) 2019 BYU PCCL see LICENSE file

#include "Holodeck.h"
#include "CustomAgent.h"


// Sets default values
ACustomAgent::ACustomAgent() {
	PrimaryActorTick.bCanEverTick = true;

	// Set the defualt controller
	AIControllerClass = LoadClass<AController>(NULL, TEXT("/Script/Holodeck.CustomAgentController"), NULL, LOAD_None, NULL);
	AutoPossessAI = EAutoPossessAI::PlacedInWorld;
}

void ACustomAgent::InitializeAgent() {
	Super::InitializeAgent();
	RootMesh = Cast<UStaticMeshComponent>(RootComponent);

	if (this->AgentMesh == "Turtlebot")
	{
		UStaticMesh* meshToUse = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/HolodeckContent/Agents/CustomAgent/Meshes/Turtlebot/TurtleBodyMesh_2.TurtleBodyMesh_2")));
		RootMesh->SetStaticMesh(meshToUse);
	}
	else if (this->AgentMesh == "Quadrotor")
	{
		UStaticMesh* meshToUse = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/HolodeckContent/Agents/CustomAgent/Meshes/UAVMesh/UAV.UAV")));
		RootMesh->SetStaticMesh(meshToUse);
	}
	else if (this->AgentMesh == "Fixed-wing UAV")
	{
		UStaticMesh* meshToUse = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/HolodeckContent/Agents/CustomAgent/Meshes/UAV-Fixed-Wing/uav-fixed-wing.uav-fixed-wing")));
		RootMesh->SetStaticMesh(meshToUse);
	}

	verifyf(RootMesh->GetStaticMesh() != nullptr, TEXT("GetStaticMesh() for CustomAgent %s returned a nullptr!"), *this->AgentName);
}

// Called every frame
void ACustomAgent::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	float ForwardForce = CommandArray[0];
	float RotForce = CommandArray[1];

	float ThrustToApply = FMath::Clamp(ForwardForce, -MAX_THRUST_CUSTOM, MAX_THRUST_CUSTOM);
	float YawTorqueToApply = FMath::Clamp(RotForce, -MAX_YAW_CUSTOM, MAX_YAW_CUSTOM);

	FVector LocalThrust = FVector(ThrustToApply, 0, 0);
	LocalThrust = ConvertLinearVector(LocalThrust, ClientToUE);

	FVector LocalTorque = FVector(0, 0, YawTorqueToApply);
	LocalTorque = ConvertTorque(LocalTorque, ClientToUE);

	RootMesh->AddTorqueInRadians(GetActorRotation().RotateVector(LocalTorque));
	RootMesh->AddForce(GetActorRotation().RotateVector(LocalThrust));

	// If the CustomAgent is upside down it is abused
	if (this->GetActorUpVector().Z < -0.5) {
		this->IsAbused = true;
	}
}
