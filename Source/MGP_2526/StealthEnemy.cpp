// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthEnemy.h"
#include "MGP_2526Character.h"
#include "Kismet/GameplayStatics.h"
#include "MGP_2526.h"
#include "DrawDebugHelpers.h"

// Sets default values
AStealthEnemy::AStealthEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStealthEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Get and store reference to player character
	PlayerCharacter = Cast<AMGP_2526Character>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
	);
	
}

// Called every frame
void AStealthEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter != nullptr)
	{
		CheckSightDetection();
		UpdateStateMachine();
	}

}

void AStealthEnemy::CheckSightDetection()
{
	// Get vector from enemy to player
	FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();

	// get distance to player
	float DistanceToPlayer = DirectionToPlayer.Size();

	// Check if player is within cone range
	if (DistanceToPlayer > ConeRange)
	{
		UpdateSuspicion(false, false, false);
		return;
	}

	// Normalize direction to player
	DirectionToPlayer.Normalize();

	// Get direction enemy is facing
	FVector EnemyForward = GetActorForwardVector();

	// Get angle between enemy forward and direction to player
	float DotProduct = FVector::DotProduct(EnemyForward, DirectionToPlayer);
	float AngleToPlayer = FMath::Acos(DotProduct) * (180.f / PI); // this is to convert from radians.... why do radians even exist... 

	// Check if player is within cone angle
	if (AngleToPlayer > ConeAngle)
	{
		UpdateSuspicion(false, false, false);
		return;
	}	

	// Perform RayCast to check for walls between enemy and player (we want to not be able to see the player through walls)
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation() + FVector(0.f, 0.f, EyeHeight);
	FVector EndLocation = PlayerCharacter->GetActorLocation();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	// Fire the raycast
	bool bHit = GetWorld()->LineTraceSingleByChannel(
    HitResult,
    StartLocation,
    EndLocation,
    ECC_Pawn,
    QueryParams
);

	// If raycast hit player, then player is visible
	if (bHit && HitResult.GetActor() == PlayerCharacter)
	{
		bool bPlayerCrouching = PlayerCharacter->bIsCrouching;
		bool bPlayerRunning = PlayerCharacter->bIsRunning;
		UpdateSuspicion(true, bPlayerCrouching, bPlayerRunning);
	}
	else
	{
		UpdateSuspicion(false, false, false);
	}

	// Debug line to visualize raycast
	
	DrawDebugLine(
		GetWorld(),
		StartLocation,
		EndLocation,
		bHit ? FColor::Green : FColor::Red,
		false,
		0.1f
);


}


void AStealthEnemy::UpdateSuspicion(bool bPlayerDetected, bool bPlayerCrouching, bool bPlayerRunning)
{
	if (bPlayerDetected)
	{
		// Reset decay timer
		SuspicionDecayTimer = 0.f;

		// Increase suspicion based on player state
		float SuspicionRate = SuspicionRateStanding;

		if (bPlayerCrouching)
		{
			SuspicionRate = SuspicionRateCrouching;
		}
		else if (bPlayerRunning)
		{
			SuspicionRate = SuspicionRateRunning;
		}

		// Increase suspicion level
		SuspicionLevel = FMath::Clamp(
			SuspicionLevel + SuspicionRate * GetWorld()->GetDeltaSeconds(),
			0.f, 1.f
		);
	}
	else
	{
		// Count up decay timer
		SuspicionDecayTimer += GetWorld()->GetDeltaSeconds();

		// If timer has expired, start decaying suspicion
		if (SuspicionDecayTimer >= SuspicionDecayDelay)
		{
			SuspicionLevel = FMath::Clamp(
				SuspicionLevel - SuspicionDecayRate * GetWorld()->GetDeltaSeconds(),
				0.f, 1.f
			);
		}
	}
}

void AStealthEnemy::UpdateStateMachine()
{
    // Transition to Alerted if suspicion is full
    if (SuspicionLevel >= 1.f)
    {
        CurrentState = EEnemyState::Alerted;
    }
    // Transition to Suspicious if suspicion is building
    else if (SuspicionLevel > 0.f)
    {
        CurrentState = EEnemyState::Suspicious;
    }
    // Return to Unaware if suspicion is empty
    else
    {
        CurrentState = EEnemyState::Unaware;
    }
}
