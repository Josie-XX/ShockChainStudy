// Fill out your copyright notice in the Description page of Project Settings.


#include "ShockEnemyGenerator.h"

#include "ShockEnemyCharacter.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
AShockEnemyGenerator::AShockEnemyGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	SetRootComponent(SceneRoot);

}

// Called when the game starts or when spawned
void AShockEnemyGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStart)
	{
		StartGeneration();
	}

}

void AShockEnemyGenerator::StartGeneration()
{
	if (bIsGenerating)
	{
		return;
	}

	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Generator: EnemyCLass is not set"));

		return;
	}

	bIsGenerating = true;

	SpawnedCount = 0;
	NextSpawnPointIndex = 0;

	OnGenerationStarted.Broadcast();

	if (GenerationMode == EEnemyGenerationMode::Single)
	{
		SpawnOneEnemy();
		FinishGeneration();
		return;
	}

	if (GenerationMode == EEnemyGenerationMode::Count)
	{
		if (SpawnCount <= 0)
		{
			FinishGeneration();
			return;
		}

		SpawnOneEnemy();

		if (SpawnedCount >= SpawnCount)
		{
			FinishGeneration();
			return;
		}
	}

	if (GenerationMode == EEnemyGenerationMode::Loop)
	{
		SpawnOneEnemy();
	}

	const float SafeInterval = FMath::Max(SpawnInterval, 0.05f);

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AShockEnemyGenerator::SpawnNextEnemy,
		SafeInterval,
		true
	);
}

void AShockEnemyGenerator::SpawnNextEnemy()
{
	if (!bIsGenerating)
	{
		return;
	}

	if (
		GenerationMode == EEnemyGenerationMode::Count &&
		SpawnedCount >= SpawnCount
		)
	{
		FinishGeneration();
		return;
	}

	SpawnOneEnemy();

	if (
		GenerationMode == EEnemyGenerationMode::Count && SpawnedCount >= SpawnCount
		)
	{
		FinishGeneration();
	}
}


void AShockEnemyGenerator::SpawnOneEnemy()
{
	if (!GetWorld() || !EnemyClass)
	{
		return;
	}

	FTransform SpawnTransform = GetActorTransform(); //没有sp的时候，在generator自己位置生成

	if (SpawnPoints.Num() > 0)
	{
		const int32 SpawnPointIndex = NextSpawnPointIndex % SpawnPoints.Num();

		if (AActor* SpawnPoint = SpawnPoints[SpawnPointIndex])
		{
			SpawnTransform = SpawnPoint->GetActorTransform();
		}

		NextSpawnPointIndex++;
	}

	FActorSpawnParameters SpawnParameters;

	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; //如果不能调整位置，也尽量生成

	AShockEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AShockEnemyCharacter>(
		EnemyClass,
		SpawnTransform,
		SpawnParameters
	);

	if (!SpawnedEnemy)
	{
		return;
	}

	SpawnedCount++;
	AliveCount++;

	SpawnedEnemy->OnEnemyDied.AddDynamic(
		this,
		&AShockEnemyGenerator::HandleEnemyDied //监听
	);

	OnEnemySpawned.Broadcast(SpawnedEnemy);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Enemy Generator: Spawned %s | Spawned=%d Alive=%d"),
		*SpawnedEnemy->GetName(),
		SpawnedCount,
		AliveCount
	);
}

void AShockEnemyGenerator::HandleEnemyDied(
	AShockEnemyCharacter* DeadEnemy
)
{
	if (DeadEnemy)
	{
		DeadEnemy->OnEnemyDied.RemoveDynamic(
			this,
			&AShockEnemyGenerator::HandleEnemyDied
		);
	}

	AliveCount = FMath::Max(AliveCount - 1, 0);

	OnEnemyDied.Broadcast(DeadEnemy);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Enemy Generator: Enemy died | Alive=%d"),
		AliveCount
	);

	if (AliveCount == 0 && !bIsGenerating)
	{
		OnAllEnemiesDead.Broadcast();
	}
}

void AShockEnemyGenerator::FinishGeneration()
{
	if (!bIsGenerating)
	{
		return;
	}

	bIsGenerating = false;

	GetWorldTimerManager().ClearTimer(
		SpawnTimerHandle
	);

	OnGenerationCompleted.Broadcast();

	if (AliveCount == 0)
	{
		OnAllEnemiesDead.Broadcast();
	}
}

void AShockEnemyGenerator::StopGeneration()
{
	if (!bIsGenerating)
	{
		return;
	}

	bIsGenerating = false;

	GetWorldTimerManager().ClearTimer(
		SpawnTimerHandle
	);

	OnGenerationCompleted.Broadcast();

	if (AliveCount == 0)
	{
		OnAllEnemiesDead.Broadcast();
	}
}

// Called every frame
void AShockEnemyGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

