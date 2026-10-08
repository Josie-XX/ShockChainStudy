// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ShieldComponent.h"

// Sets default values for this component's properties
UShieldComponent::UShieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UShieldComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentShield = MaxShield;

	// ...
	
}

bool UShieldComponent::HasShield() const
{
	return CurrentShield > 0.0f;
}

//返回没有被护盾吸收的伤害
float UShieldComponent::AbsorbDamage( 
	float DamageAmount,
	bool bIsShockDamage
)
{
	if (DamageAmount <= 0.0f || CurrentShield <= 0.0f)
	{
		return DamageAmount;
	}

	const float Multiplier = bIsShockDamage ? ShockDamageMultiplier : 1.0f;
	const float ModifiedDamage = DamageAmount * Multiplier;
	const float ShieldDamage = FMath::Min(CurrentShield, ModifiedDamage); //消耗的护盾

	CurrentShield -= ShieldDamage;

	const float ShieldNormalized = MaxShield > 0.0f ? CurrentShield / MaxShield : 0.0f; //护盾比例

	OnShieldChanged.Broadcast(ShieldNormalized);

	const float RawDamageAbsorbed = ShieldDamage / Multiplier; 
	const float RemainingDamage = DamageAmount - RawDamageAbsorbed; //被护盾挡掉的以外的伤害

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Shield: %.1f / %.1f | Damage x%.1f"),
		CurrentShield,
		MaxShield,
		Multiplier
	);

	return FMath::Max(RemainingDamage, 0.0f);
}


// Called every frame
void UShieldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

