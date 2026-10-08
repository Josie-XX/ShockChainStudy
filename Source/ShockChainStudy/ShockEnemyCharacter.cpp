// Fill out your copyright notice in the Description page of Project Settings.


#include "ShockEnemyCharacter.h"

#include "Components/HealthComponent.h"
#include "Components/ShieldComponent.h"
#include "Combat/DamageTypes/ShockDamageType.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"

#include "ShockEnemyAIController.h"
#include "AIController.h"

// Sets default values
AShockEnemyCharacter::AShockEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent =
		CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	ShieldComponent =
		CreateDefaultSubobject<UShieldComponent>(TEXT("ShieldComponent"));

	AIControllerClass = AShockEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 250.0f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECC_Visibility,
		ECR_Block
	);

}

// Called when the game starts or when spawned
void AShockEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic( //绑定health
			this,
			&AShockEnemyCharacter::HandleDeath
		);
	}
	
}

// Take Damage
float AShockEnemyCharacter::TakeDamage(
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

	const bool bIsShockDamage =
		DamageEvent.DamageTypeClass &&
		DamageEvent.DamageTypeClass->IsChildOf(
			UShockDamageType::StaticClass()
		);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Direct Hit DamageType = %s | IsShock = %s"),
		DamageEvent.DamageTypeClass
		? *DamageEvent.DamageTypeClass->GetName()
		: TEXT("None"),
		bIsShockDamage ? TEXT("TRUE") : TEXT("FALSE")
	);

	if (bIsShockDamage)
	{
		OnShockHit();
		OnShockBodyEffect();
	}

	float RemainingDamage = ActualDamage;

	if (ShieldComponent && ShieldComponent->HasShield())
	{
		RemainingDamage = ShieldComponent->AbsorbDamage(
			RemainingDamage,
			bIsShockDamage
		);
	}

	if (HealthComponent && RemainingDamage > 0.0f)
	{
		HealthComponent->ApplyHealthDamage(RemainingDamage);
	}

	return ActualDamage;
}

float AShockEnemyCharacter::GetMovementSpeed() const
{
	return GetVelocity().Size2D();
}

// Called every frame
void AShockEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShockEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AShockEnemyCharacter::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	OnEnemyDied.Broadcast(this);

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	BP_PlayDeathPresentation();

	SetLifeSpan(DestroyDelay);
}
