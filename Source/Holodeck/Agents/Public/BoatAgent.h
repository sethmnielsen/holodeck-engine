// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HolodeckCore/Public/HolodeckAgent.h"
#include "Holodeck.h"
#include "BoatAgent.generated.h"

/**
 * 
 */
UCLASS()
class HOLODECK_API ABoatAgent : public AHolodeckAgent
{
	GENERATED_BODY()
	
public:
	/**
	* Default Constructor.
	*/
	ABoatAgent();

	/**
	* BeginPlay
	* Called when the game starts.
	* Registers the reward and terminal signals.
	*/
	void InitializeAgent() override;

	/**
	* Tick
	* Called each frame.
	* @param DeltaSeconds the time since the last tick.
	*/
	void Tick(float DeltaSeconds) override;

	unsigned int GetRawActionSizeInBytes() const override { return sizeof(float) * 2; };
	void* GetRawActionBuffer() const override { return (void*)CommandArray; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

private:
	float CommandArray[2];
	
};
