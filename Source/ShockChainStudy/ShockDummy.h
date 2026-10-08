// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShockDummy.generated.h"

class UStaticMeshComponent;
class UHealthComponent;
class UShieldComponent;

UCLASS()
class SHOCKCHAINSTUDY_API AShockDummy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShockDummy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UShieldComponent* ShieldComponent;

	UFUNCTION(BlueprintImplementableEvent, Category = "Feedback")
	void OnShockHit();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
