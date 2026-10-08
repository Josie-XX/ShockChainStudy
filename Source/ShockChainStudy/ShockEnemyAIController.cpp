// Fill out your copyright notice in the Description page of Project Settings.


#include "ShockEnemyAIController.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

void AShockEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Check
	UE_LOG(LogTemp, Warning, TEXT("AI Possessed: %s"), *GetNameSafe(InPawn));

	TryMoveToPlayer();

}

void AShockEnemyAIController::TryMoveToPlayer()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0); //找到第一个Player Pawn

	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("AI: PlayerPawn NOT FOUND"));

		GetWorldTimerManager().SetTimer(
			PlayerSearchTimerHandle,
			this,
			&AShockEnemyAIController::TryMoveToPlayer,
			0.1f,
			false
		);

		return;
	}

	const EPathFollowingRequestResult::Type MoveResult = MoveToActor(PlayerPawn, AcceptanceRadius); //在NavMesh上寻找一条可以走到Player的路径，AcceptanceRadius：还有180就认为达到

	UE_LOG(LogTemp, Warning, TEXT("MoveToActor result = %d | Player = %s"), static_cast<int32>(MoveResult), *GetNameSafe(PlayerPawn));
}
