// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShockChainComponent.generated.h"

class AController;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnChainVFXRequested,
	FVector, StartLocation,
	FVector, EndLocation
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOCKCHAINSTUDY_API UShockChainComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShockChainComponent();

	UPROPERTY(BlueprintAssignable, Category = "Shock Chain|VFX")
	FOnChainVFXRequested OnChainVFXRequested;

	//需要让外部知道，需要执行连锁
	void TriggerChain(
		const FVector& ChainOrigin,
		AActor* PrimaryTarget,
		float PrimaryDamage,
		AController* EventInstigator,
		AActor* DamageCauser
	);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shock Chain")
	float ChainRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shock Chain")
	int32 MaxChainTargets = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shock Chain")
	float ChainDamageMultiplier = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shock Chain")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shock Chain")
	float ChainStepDelay = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shock Chain|VFX")
	TObjectPtr<UNiagaraSystem> ChainVFX;



public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	void SpawnChainVFX(
		const FVector& StartLocation,
		const FVector& EndLocation
	);
};
