// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ShockChainComponent.h"

#include "ShockDummy.h"
#include "Combat/DamageTypes/ShockDamageType.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "TimerManager.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "ShockEnemyCharacter.h"


// Sets default values for this component's properties
UShockChainComponent::UShockChainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UShockChainComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UShockChainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UShockChainComponent::SpawnChainVFX(
	const FVector& StartLocation,
	const FVector& EndLocation
)
{
	OnChainVFXRequested.Broadcast(
		StartLocation,
		EndLocation
	);

	/*
	if (!GetWorld() || !ChainVFX)
	{
		return;
	}

	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		ChainVFX,
		StartLocation,
		FRotator::ZeroRotator,
		FVector::OneVector,
		true,
		false //创建出后先不播放，先设置beam修改参数
	);

	if (!NiagaraComponent)
	{
		return;
	}

	NiagaraComponent->SetVariableVec3(TEXT("User.BeamStart"), StartLocation);
	NiagaraComponent->SetVariableVec3(TEXT("User.BeamEnd"), EndLocation);

	NiagaraComponent->Activate(true);
	*/
}


void UShockChainComponent::TriggerChain(
	const FVector& ChainOrigin, //第一枪命中
	AActor* PrimaryTarget, //打中的actor
	float PrimaryDamage, //基础伤害
	AController* EventInstigator, //哪个Control攻击 （PlayerController）
	AActor* DamageCauser //攻击的Actor是谁 （Player1）
)
{
	if (!GetWorld() || !PrimaryTarget)
	{
		return;
	}

	//Overlap查找对象范围，用OverlapResults储存结果
	//不查找所有敌人并一一计算距离，只需考虑范围内即可
	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;  //Overlap搜索哪些Collision Object
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); //墙，地板，建筑等
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); //变化的世界物体，Actor，交互物等
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn); //Player，Enemy， Character，AI Pawn等

	FCollisionQueryParams QueryParams; //搜索的特殊规则
	QueryParams.AddIgnoredActor(GetOwner()); //忽略Player本人
	QueryParams.AddIgnoredActor(PrimaryTarget); //忽略初始对象

	GetWorld()->OverlapMultiByObjectType( //进行Overlap搜索，返回多个结果
		OverlapResults, //输出
		ChainOrigin, //命中中心
		FQuat::Identity, //没有旋转
		ObjectQueryParams, //搜索对象
		FCollisionShape::MakeSphere(ChainRadius), //用球形来查找
		QueryParams //搜索规则
	);

	TArray<AShockEnemyCharacter*> CandidateTargets; //我们想要的target

	for (const FOverlapResult& Result : OverlapResults) //对所有overlap的result
	{
		if (AShockEnemyCharacter* Enemy = Cast<AShockEnemyCharacter>(Result.GetActor())) //如果result的actor能被当作Enemy，就可以
		{
			CandidateTargets.AddUnique(Enemy); //如果candidate里没有就添加，且只添加一次，不会把不同的部件考虑进去
		}
	}

	CandidateTargets.Sort( //排序，最近的敌人优先
		[&ChainOrigin](const AShockEnemyCharacter& A, const AShockEnemyCharacter& B) //C++ lambda临时函数
		{
			return FVector::DistSquared(A.GetActorLocation(), ChainOrigin)
				< FVector::DistSquared(B.GetActorLocation(), ChainOrigin);
		}
	);

	const int32 NumTargets = FMath::Min(MaxChainTargets, CandidateTargets.Num()); //最多能chain几个
	const float ChainDamage = PrimaryDamage * ChainDamageMultiplier;

	TWeakObjectPtr<AActor> PreviousTarget = PrimaryTarget;

	for (int32 i = 0; i < NumTargets; i++)
	{
		TWeakObjectPtr<AShockEnemyCharacter> Target = CandidateTargets[i];//每次选一个target，并且在0.12秒里保证target存在

		const float Delay = ChainStepDelay * (i + 1); //设置一个连锁的delay

		TWeakObjectPtr<AActor> LinkStartTarget = PreviousTarget;

		FTimerHandle TimerHandle;

		FTimerDelegate ChainDelegate = FTimerDelegate::CreateWeakLambda(
			this,
			[this,
			Target,
			LinkStartTarget,
			ChainOrigin,
			ChainDamage,
			EventInstigator,
			DamageCauser]()
			{
				if (!Target.IsValid())
				{
					return;
				}

				const FVector StartLocation = LinkStartTarget.IsValid() ? LinkStartTarget->GetActorLocation() : ChainOrigin;

				const FVector EndLocation = Target->GetActorLocation();

				UGameplayStatics::ApplyDamage(
					Target.Get(),
					ChainDamage,
					EventInstigator,
					DamageCauser,
					UShockDamageType::StaticClass() //标记damage是shock的
				);

				UE_LOG(
					LogTemp,
					Warning,
					TEXT("Chain Shock -> %s | Damage: %.1f"),
					*Target->GetName(),
					ChainDamage
				);

				SpawnChainVFX(StartLocation, EndLocation);

				if (bDrawDebug) //Debug的时候画线
				{
					DrawDebugLine(
						GetWorld(),
						ChainOrigin,
						Target->GetActorLocation(),
						FColor::Cyan,
						false,
						0.35f,
						0,
						3.0f
					);
				}
			}
		);

		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			ChainDelegate,
			Delay,
			false
		);

		PreviousTarget = CandidateTargets[i];

	}

	if (bDrawDebug) //Debug时候画范围
	{
		DrawDebugSphere(
			GetWorld(),
			ChainOrigin,
			ChainRadius,
			32,
			FColor::Cyan,
			false,
			1.0f
		);
	}
}


