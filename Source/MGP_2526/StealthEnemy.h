// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StealthEnemy.generated.h"

class AMGP_2526Character;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Unaware UMETA(DisplayName = "Unaware"),
	Suspicious UMETA(DisplayName = "Suspicious"),
	Alerted UMETA(DisplayName = "Alerted")

};

UCLASS()
class MGP_2526_API AStealthEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStealthEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Current enemy state */
	UPROPERTY(VisibleAnywhere, Category = "Detection")
	EEnemyState CurrentState = EEnemyState::Unaware;

	/** Half angle of the detection cone in degrees */
	UPROPERTY(EditAnywhere, Category="Detection")
	float ConeAngle = 45.f;
	
	/** Maximum range of the detection cone */
	UPROPERTY(EditAnywhere, Category="Detection")
	float ConeRange = 1000.f;
	
	/** Radius for hearing detection */
	UPROPERTY(EditAnywhere, Category="Detection")
	float ProximityRange = 300.f;
	
	/** How fast suspicion fills when player is standing in cone */
	UPROPERTY(EditAnywhere, Category="Detection")
	float SuspicionRateStanding = 0.4f;
	
	/** How fast suspicion fills when player is crouching in cone */
	UPROPERTY(EditAnywhere, Category="Detection")
	float SuspicionRateCrouching = 0.1f;
	
	/** How fast suspicion drains when player is out of range */
	UPROPERTY(EditAnywhere, Category="Detection")
	float SuspicionDecayRate = 0.2f;

	/** How fast suspicion fills when player is running in cone */
	UPROPERTY(EditAnywhere, Category="Detection")
	float SuspicionRateRunning = 0.7f;
	
	/** Current suspicion level between 0 and 1 */
	UPROPERTY(VisibleAnywhere, Category="Detection")
	float SuspicionLevel = 0.f;

	/** Vertical offset for sight detection */
	UPROPERTY(EditAnywhere, Category="Detection")
	float EyeHeight = 64.f;  // start from enemy's eyes... googled "character eye height unreal" and 64 seemed to be it 

	/** Updates the enemy FSM based on current suspicion level (lol finally we get to finite state machines!!!) */
	void UpdateStateMachine();
	
	/** Checks if player is within the detection cone and visible */
	void CheckSightDetection();
	
	/** Checks if player is within hearing range */
	void CheckHearingDetection();

	/** Updates suspicion level based on detection */
	void UpdateSuspicion(bool bPlayerDetected, bool bPlayerCrouching, bool bPlayerRunning);

	/** How long the enemy will wait before suspicion starts to decay */
	UPROPERTY(EditAnywhere, Category="Detection")
	float SuspicionDecayDelay = 2.f;

	/** Current delay timer before suspicion starts to decay */
	UPROPERTY(VisibleAnywhere, Category="Detection")
	float SuspicionDecayTimer = 0.f;

	/** Reference to the player character */
	UPROPERTY()
	AMGP_2526Character* PlayerCharacter;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
