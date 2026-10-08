// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShockChainStudyCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShockChainStudy.h"
#include "DrawDebugHelpers.h"

#include "ShockDummy.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/DamageTypes/ShockDamageType.h"
#include "Components/ShockChainComponent.h"

#include "ShockEnemyCharacter.h"

#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AShockChainStudyCharacter::AShockChainStudyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	//visual muzzle origin
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));

	MuzzlePoint->SetupAttachment(FirstPersonCameraComponent);

	MuzzlePoint->SetRelativeLocation( //子弹发射源点
		FVector(60.0f, 20.0f, -20.0f)
	);

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	//创建ShockChain的Blueprint
	ShockChainComponent = CreateDefaultSubobject<UShockChainComponent>(TEXT("ShockChainComponent"));
}

void AShockChainStudyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShockChainStudyCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AShockChainStudyCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShockChainStudyCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShockChainStudyCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AShockChainStudyCharacter::LookInput);

		//Fire
		EnhancedInputComponent->BindAction(RailgunFireAction, ETriggerEvent::Started, this, &AShockChainStudyCharacter::Fire);
	}
	else
	{
		UE_LOG(LogShockChainStudy, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AShockChainStudyCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AShockChainStudyCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AShockChainStudyCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AShockChainStudyCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AShockChainStudyCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AShockChainStudyCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AShockChainStudyCharacter::Fire()
{
	FVector Start = FirstPersonCameraComponent->GetComponentLocation(); //射击起点
	FVector Forward = FirstPersonCameraComponent->GetForwardVector(); //射击方向
	FVector End = Start + Forward * 10000.0f; //100米

	FHitResult HitResult; //保存命中结果
	FCollisionQueryParams QueryParams; //碰撞查询设置

	QueryParams.AddIgnoredActor(this);//无法打到自己

	bool bHit = GetWorld()->LineTraceSingleByChannel( //发射射线
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	FVector TracerEnd = End;

	if (bHit)
	{
		TracerEnd = HitResult.ImpactPoint;
	}

	const FVector TracerStart = MuzzlePoint ? MuzzlePoint->GetComponentLocation() : Start;

	SpawnBulletTracer(
		TracerStart,
		TracerEnd
	);
	
	/*
	DrawDebugLine( //画出射线
		GetWorld(),
		Start,
		End,
		FColor::Red,
		false,
		2.0f,
		0,
		1.0f
	);
	*/

	//Damage判断
	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();

		if (AShockEnemyCharacter* ShockEnemy = Cast<AShockEnemyCharacter>(HitActor)) //Cast - 这个Actor是不是Dummy系的
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy Hit!"));

			const float PrimaryDamage = 25.0f;

			UGameplayStatics::ApplyDamage(
				ShockEnemy,
				PrimaryDamage,
				GetController(),
				this,
				UShockDamageType::StaticClass()
			);

			if (ShockChainComponent)
			{
				ShockChainComponent->TriggerChain(
					HitResult.ImpactPoint,
					ShockEnemy,
					PrimaryDamage,
					GetController(),
					this
				);
			}
		}
		else 
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Hit: %s"),
				*HitResult.GetActor()->GetName()
			);
		}

	}
}

void AShockChainStudyCharacter::SpawnBulletTracer(
	const FVector& TracerStart,
	const FVector& TracerEnd
)
{
	if (!GetWorld() || !BulletTracerVFX)
	{
		return;
	}

	const float SafeTravelTime = FMath::Max(
		BulletTracerTravelTime,
		0.01f
	);

	const FVector TracerVelocity = (TracerEnd - TracerStart) / SafeTravelTime;

	UNiagaraComponent* TracerComponent =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), //世界
			BulletTracerVFX, //选哪个Niagara
			TracerStart, //在枪口附近
			FRotator::ZeroRotator, //不旋转
			FVector::OneVector, //决定Scale
			true, //自动销毁
			false //不自动activate
		);

	if (!TracerComponent)
	{
		return;
	}

	TracerComponent->SetVariableVec3(
		TEXT("User.TracerVelocity"),
		TracerVelocity
	);

	TracerComponent->Activate(true);
}