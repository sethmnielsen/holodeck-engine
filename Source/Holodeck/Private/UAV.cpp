// Fill out your copyright notice in the Description page of Project Settings.
#include "Holodeck.h"
#include "UAV.h"
#include <math.h>

AUAV::AUAV()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the defualt controller
	AIControllerClass = LoadClass<AController>(NULL, TEXT("/Script/Holodeck.HolodeckUAVController"), NULL, LOAD_None, NULL);
	AutoPossessAI = EAutoPossessAI::PlacedInWorld;

	// TODO: the unreal unit conversion should be derived via global settings
	// TODO: the physics substep doesn't seem to have updated velocity/position estimates - we likely need to access the physx body directly (e.g. https://github.com/EpicGames/UnrealEngine/pull/585)
	// TODO: accept desired roll/pitch/yaw/altitude commands via HolodeckMessage
	// TODO: add changes seen in https://answers.unrealengine.com/questions/7459/question-is-120-the-engine-max-frame-rate.html
	// TODO: set deltaTick to 1/40th of a second

	SetActorEnableCollision(true);
	OnCalculateCustomPhysics.BindUObject(this, &AUAV::SubstepTick);
}

// Called when the game starts or when spawned
void AUAV::BeginPlay()
{
	Super::BeginPlay();

	RootMesh = Cast<UStaticMeshComponent>(RootComponent);
		
	// Set up the PID Controllers TODO - WHAT TO USE FOR TAU?
	RollController.setGains(UAV_ROLL_P, UAV_ROLL_I, UAV_ROLL_D, UAV_TAU_DOWN_ROLL_L);
	PitchController.setGains(UAV_PITCH_P, UAV_PITCH_I, UAV_PITCH_D, UAV_TAU_DOWN_PITCH_M);
	YawController.setGains(UAV_YAW_P, UAV_YAW_I, UAV_YAW_D, UAV_TAU_DOWN_YAW_RATE_N);
	AltitudeController.setGains(UAV_ALT_P, UAV_ALT_I, UAV_ALT_D, UAV_TAU_DOWN_FORCE_F);
}

// Called every frame
void AUAV::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	if(RootMesh){
		RootMesh->GetBodyInstance()->AddCustomPhysics(OnCalculateCustomPhysics);
	}
}
	
void AUAV::SubstepTick(float DeltaTime, FBodyInstance* BodyInstance)
{
	UpdateForcesAndMoments(DeltaTime);
	ApplyForces();
}

FVector AUAV::RotatorToEulerInZYX(FRotator Rotator) {
	FQuat Quaternion = Rotator.Quaternion();

	float Roll = atan2(2.0 * (Quaternion.W * Quaternion.X + Quaternion.Y * Quaternion.Z), 1.0 - 2.0 * (Quaternion.X * Quaternion.X + Quaternion.Y * Quaternion.Y));
	float Pitch = asin(2.0 * (Quaternion.W * Quaternion.Y - Quaternion.Z * Quaternion.X));
	float Yaw = atan2(2.0 * (Quaternion.W * Quaternion.Z + Quaternion.X * Quaternion.Y), 1.0 - 2.0 * (Quaternion.Y * Quaternion.Z + Quaternion.Z * Quaternion.Z));

	return FVector(Roll, Pitch, Yaw);
}

void AUAV::UpdateForcesAndMoments(float DeltaTime)
{
	// Get the current locations
	CurrentPositionX = UEUnitsToMeters(GetActorLocation().X);
	CurrentPositionY = UEUnitsToMeters(GetActorLocation().Y);
	CurrentPositionZ = UEUnitsToMeters(GetActorLocation().Z);

	// Get the rotator to get state and transform from world to local coordinates
	FRotator CurrentRotator = GetActorRotation();
	FVector EulerRotation = RotatorToEulerInZYX(CurrentRotator);  // Get these in local coords in (Z, Y, X) order - CurrentRotator.Euler() provides (X, Y, Z)
	FVector CurrentGlobalVelocity = GetVelocity();
	FVector LocalAngularVelocity = CurrentRotator.UnrotateVector(RootMesh->GetPhysicsAngularVelocity());

	CurrentRoll = EulerRotation.X;
	CurrentPitch = EulerRotation.Y;
	CurrentYaw = EulerRotation.Z;
	CurrentGlobalVelocityZ = UEUnitsToMeters(CurrentGlobalVelocity.Z);

	float CurrentRollRate = FMath::DegreesToRadians(LocalAngularVelocity.X);
	float CurrentPitchRate = FMath::DegreesToRadians(LocalAngularVelocity.Y);
	CurrentYawRate = FMath::DegreesToRadians(LocalAngularVelocity.Z);

	// Convert from [North, West, Up] to [North, East, Down] coordinate frames
	CurrentPitch *= -1;
	CurrentYaw *= -1;
	CurrentPitchRate *= -1;
	CurrentYawRate *= -1;
	
	// Calculate the force and torques from the PID controller
	RollTorqueToApply = RollController.computePIDDirect(DesiredRoll, CurrentRoll, CurrentRollRate, DeltaTime);
	PitchTorqueToApply = PitchController.computePIDDirect(DesiredPitch, CurrentPitch, CurrentPitchRate, DeltaTime);
	YawTorqueToApply = YawController.computePID(DesiredYawRate, CurrentYawRate, DeltaTime);

	float HoverThrust = (RootMesh->GetMass() * -UEUnitsToMeters(GWorld->GetGravityZ())) / (cos(DesiredRoll) * cos(DesiredPitch));
	ThrustToApply = AltitudeController.computePIDDirect(DesiredAltitude, CurrentPositionZ, CurrentGlobalVelocityZ, DeltaTime) + HoverThrust;

	// Calculate first-order filter
	float TauRoll =    (RollTorqueToApply > CurrentRollTorque) ? UAV_TAU_UP_ROLL_L : UAV_TAU_DOWN_ROLL_L;
	float TauPitch =   (PitchTorqueToApply > CurrentPitchTorque) ? UAV_TAU_UP_PITCH_M : UAV_TAU_DOWN_PITCH_M;
	float TauYawRate = (YawTorqueToApply > CurrentYawTorque) ? UAV_TAU_UP_YAW_RATE_N : UAV_TAU_DOWN_YAW_RATE_N;
	float TauThrust =  (ThrustToApply > CurrentThrust) ? UAV_TAU_UP_FORCE_F : UAV_TAU_DOWN_FORCE_F;
	float AlphaRoll = DeltaTime / (TauRoll + DeltaTime);
	float AlphaPitch = DeltaTime / (TauPitch + DeltaTime);
	float AlphaYaw = DeltaTime / (TauYawRate + DeltaTime);
	float AlphaThrust = DeltaTime / (TauThrust + DeltaTime);

	RollTorqueToApply = (1 - AlphaRoll) * RollTorqueToApply + AlphaRoll * RollTorqueToApply;
	PitchTorqueToApply = (1 - AlphaPitch) * PitchTorqueToApply + AlphaPitch * PitchTorqueToApply;
	YawTorqueToApply = (1 - AlphaRoll) * YawTorqueToApply + AlphaYaw * YawTorqueToApply;
	ThrustToApply = (1 - AlphaThrust) * ThrustToApply + AlphaThrust * ThrustToApply;

	// Calculate Air Resistance
	Wind = -UAV_MU * CurrentGlobalVelocity;

	// Apply the discrete first order filter
	RollTorqueToApply = FMath::Clamp(RollTorqueToApply, -UAV_MAX_ROLL_L, UAV_MAX_ROLL_L);
	PitchTorqueToApply = FMath::Clamp(PitchTorqueToApply, -UAV_MAX_PITCH_M, UAV_MAX_PITCH_M);
	YawTorqueToApply = FMath::Clamp(YawTorqueToApply, -UAV_MAX_YAW_RATE_N, UAV_MAX_YAW_RATE_N);
	ThrustToApply = FMath::Clamp(ThrustToApply, -UAV_MAX_FORCE_F, UAV_MAX_FORCE_F);
}

float AUAV::UEUnitsToMeters(float ValueInUnrealUnits) {
	return ValueInUnrealUnits / 100.0;
}

void AUAV::ApplyForces()
{	
	FVector LocalThrust = FVector(0, 0, ThrustToApply); 
	FVector LocalTorque = FVector(RollTorqueToApply, -PitchTorqueToApply, -YawTorqueToApply); // In [North, West, Up] format

	// Convert meters to unreal-unit-newtons
	LocalThrust *= 100;
	LocalTorque *= 10000;

	// Scale force for substepping
	LocalThrust /= 16;
	LocalTorque /= 16;

	// Apply torques and forces in global coordinates
	RootMesh->AddTorque(GetActorRotation().RotateVector(LocalTorque));
	RootMesh->AddForce(GetActorRotation().RotateVector(LocalThrust));
	RootMesh->AddForce(Wind / 16);

	CurrentRollTorque = RollTorqueToApply;
	CurrentPitchTorque = PitchTorqueToApply;
	CurrentYawTorque = YawTorqueToApply;
	CurrentThrust = ThrustToApply;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Wind.ToString());/*
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Wind: %f, Velocity: %f"), Wind.ToString(), GetVelocity().ToString()));*/
}
