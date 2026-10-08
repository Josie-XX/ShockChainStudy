// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TimerManager.h"
#include "ShockEnemyAIController.generated.h"



/**
 * 
 */
UCLASS()
class SHOCKCHAINSTUDY_API AShockEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override; //AI正式接管的时刻

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AcceptanceRadius = 180.0f;

private:
	void TryMoveToPlayer();

	FTimerHandle PlayerSearchTimerHandle;
};
