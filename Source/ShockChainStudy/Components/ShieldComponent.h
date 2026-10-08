// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShieldComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnShieldChanged,
	float,
	ShieldNormalized
);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOCKCHAINSTUDY_API UShieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShieldComponent();

	UPROPERTY(BlueprintAssignable, Category = "Shield|Events")
	FOnShieldChanged OnShieldChanged;

	float AbsorbDamage(float DamageAmount, bool bIsShockDamage);
	bool HasShield() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	float MaxShield = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	float CurrentShield = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	float ShockDamageMultiplier = 2.0f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
