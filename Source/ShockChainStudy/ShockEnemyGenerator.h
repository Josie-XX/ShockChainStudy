// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShockEnemyGenerator.generated.h"

class AShockEnemyCharacter;
class USceneComponent;

//生成模式
UENUM(BlueprintType)
enum class EEnemyGenerationMode : uint8
{
	Single UMETA(DisplayName = "Single"),
	Count UMETA(DisplayName = "Count"),
	Loop UMETA(DisplayName = "Loop")
};

//生命周期
DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FShockGenerationStartedSignature
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FShockEnemySpawnedSignature,
	AShockEnemyCharacter*,
	SpawnedEnemy
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FShockGeneratedEnemyDiedSignature,
	AShockEnemyCharacter*,
	DeadEnemy
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FShockGenerationCompletedSignature
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FShockAllEnemiesDeadSignature
);

UCLASS()
class SHOCKCHAINSTUDY_API AShockEnemyGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShockEnemyGenerator();

	UFUNCTION(BlueprintCallable, Category = "Enemy Generator")
	void StartGeneration();

	UFUNCTION(BlueprintCallable, Category = "Enemy Generator")
	void StopGeneration();

	UPROPERTY(BlueprintAssignable, Category = "Enemy Generator|Events")
	FShockGenerationStartedSignature OnGenerationStarted;

	UPROPERTY(BlueprintAssignable, Category = "Enemy Generator|Events")
	FShockEnemySpawnedSignature OnEnemySpawned;

	UPROPERTY(BlueprintAssignable, Category = "Enemy Generator|Events")
	FShockGeneratedEnemyDiedSignature OnEnemyDied;

	UPROPERTY(BlueprintAssignable, Category = "Enemy Generator|Events")
	FShockGenerationCompletedSignature OnGenerationCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Enemy Generator|Events")
	FShockAllEnemiesDeadSignature OnAllEnemiesDead;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Generator")
	EEnemyGenerationMode GenerationMode = EEnemyGenerationMode::Count;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Generator")
	TSubclassOf<AShockEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Generator")
	int32 SpawnCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Generator")
	float SpawnInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Generator")
	bool bAutoStart = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Enemy Generator")
	TArray<TObjectPtr<AActor>> SpawnPoints;

private:
	void SpawnNextEnemy();
	void SpawnOneEnemy();
	void FinishGeneration();

	UFUNCTION()
	void HandleEnemyDied(AShockEnemyCharacter* DeadEnemy);

	FTimerHandle SpawnTimerHandle;
	int32 SpawnedCount = 0;
	int32 AliveCount = 0;
	int32 NextSpawnPointIndex = 0;
	bool bIsGenerating = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
