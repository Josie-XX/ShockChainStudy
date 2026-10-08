// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShockEnemyCharacter.generated.h"

class UHealthComponent;
class UShieldComponent;
class AShockEnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnShockEnemyDied,
	AShockEnemyCharacter*,
	DeadEnemy
);

UCLASS()
class SHOCKCHAINSTUDY_API AShockEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShockEnemyCharacter();

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UFUNCTION(BlueprintPure, Category = "Movement")
	float GetMovementSpeed() const;

	UPROPERTY(BlueprintAssignable, Category = "Enemy|Events")
	FOnShockEnemyDied OnEnemyDied;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UShieldComponent* ShieldComponent;

	UFUNCTION(BlueprintImplementableEvent, Category = "Feedback")
	void OnShockHit();

	UFUNCTION(BlueprintImplementableEvent, Category = "Feedback")
	void OnShockBodyEffect();

	//监听Death
	UFUNCTION()
	void HandleDeath();

	//死亡表现
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Death")
	void BP_PlayDeathPresentation();

	//死亡销毁
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Death")
	float DestroyDelay = 2.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	bool bIsDead = false;

};
