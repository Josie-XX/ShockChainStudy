// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bDeathBroadcasted = false;

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//减少实际damage
float UHealthComponent::ApplyHealthDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	if (bDeathBroadcasted)
	{
		return 0.0f;
	}

	const float OldHealth = CurrentHealth;

	CurrentHealth = FMath::Clamp(
		CurrentHealth - DamageAmount,
		0.0f,
		MaxHealth
	);

	const float ActualDamage = OldHealth - CurrentHealth;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Health: %.1f / % .1f"),
		CurrentHealth,
		MaxHealth
	);

	if (CurrentHealth <= 0.0f && !bDeathBroadcasted)
	{
		bDeathBroadcasted = true;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Health reached zero -> OnDeath Broadcast")
		)
		
		OnDeath.Broadcast();
	}

	return ActualDamage;
}

