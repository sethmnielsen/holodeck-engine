// MIT License (c) 2019 BYU PCCL see LICENSE file

#pragma once

#include "Runtime/CoreUObject/Public/UObject/SoftObjectPtr.h"
#include "GameFramework/Pawn.h"
#include "HolodeckAgent.h"
#include "CustomAgent.generated.h"


static constexpr  float MAX_THRUST_CUSTOM = 160.0f;
static constexpr  float MAX_YAW_CUSTOM = 35.0f;

UCLASS()
/**
* ACustomAgent
* Inherits from the HolodeckAgent class
* On any tick this object will apply the given forces.
* Desired values must be set by a controller.
*/
class HOLODECK_API ACustomAgent : public AHolodeckAgent
{
	GENERATED_BODY()

public:
	/**
	* Default Constructor.
	*/
	ACustomAgent();

	void InitializeAgent() override;

	/**
	* Tick
	* Called each frame.
	* @param DeltaSeconds the time since the last tick.
	*/
	void Tick(float DeltaSeconds) override;

	// UPROPERTY( Category="CustomStaticMesh", BlueprintReadWrite, meta=( DisplayName="Set Custom Mesh", AllowedClasses="StaticMesh" ) )
		// UStaticMeshComponent* RootMesh;
	// CustomMesh.
	// CustomMesh.SetPath("/home/seth/Documents/unreal_projects/meshes/intergalactic_spaceship.fbx");
	// MeshForLoad = Cast<UStaticMesh>( CustomMesh.ResolveObject() );

	UPROPERTY(BlueprintReadWrite, Category="UAVMesh")
		UStaticMeshComponent* RootMesh;

	unsigned int GetRawActionSizeInBytes() const override { return 2 * sizeof(float); };
	void* GetRawActionBuffer() const override { return (void*)CommandArray; };

	// Allows agent to fall up to ~8 meters
	float GetAccelerationLimit() override { return 400; }

private:
	/**
	* 0: ThrustToApply
	* 1: YawTorqueToApply
	*/
	float CommandArray[2];

};
