// Fill out your copyright notice in the Description page of Project Settings.


#include "ShockDummy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ShieldComponent.h"
#include "Combat/DamageTypes/ShockDamageType.h"
#include "Engine/DamageEvents.h"

// Sets default values
AShockDummy::AShockDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh")); //创建一个组件，Mesh是Dummy空间的主要对象
	SetRootComponent(Mesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	ShieldComponent = CreateDefaultSubobject<UShieldComponent>(TEXT("ShieldComponent"));
}

// Called when the game starts or when spawned
void AShockDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShockDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//承受伤害函数
float AShockDummy::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	const float ActualDamage = Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser
	);

	const bool bIsShockDamage = DamageEvent.DamageTypeClass && DamageEvent.DamageTypeClass->IsChildOf(UShockDamageType::StaticClass());

	if (bIsShockDamage)
	{
		OnShockHit();
	}

	float RemainingDamage = ActualDamage;

	if (ShieldComponent && ShieldComponent->HasShield())
	{
		RemainingDamage = ShieldComponent->AbsorbDamage(RemainingDamage, bIsShockDamage);
	}

	if (HealthComponent && RemainingDamage > 0.0f)
	{
		HealthComponent->ApplyHealthDamage(RemainingDamage);
	}
	
	return ActualDamage;
}
