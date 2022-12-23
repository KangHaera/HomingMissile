// Copyright Epic Games, Inc. All Rights Reserved.

#include "HMProjectile.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "HomingMissile/Monster/HMMonster.h"

AHMProjectile::AHMProjectile() 
{
	//	 구 생성
	m_CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	m_CollisionComp->InitSphereRadius(5.0f);
	m_CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	m_CollisionComp->OnComponentHit.AddDynamic(this, &AHMProjectile::CallBack_OnHit);
	m_CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	m_CollisionComp->CanCharacterStepUpOn = ECB_No;

	//	root로 지정
	RootComponent = m_CollisionComp;

	//	mesh 생성
	m_StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	m_StaticMeshComp->SetupAttachment(RootComponent);

	//	레디얼포스 생성
	m_RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	m_RadialForceComp->SetupAttachment(RootComponent);

	//	프로젝타일 무브먼트 생성
	m_ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	m_ProjectileMovementComp->UpdatedComponent = m_CollisionComp;
	m_ProjectileMovementComp->InitialSpeed = 3000.f;
	m_ProjectileMovementComp->MaxSpeed = 3000.f;
	m_ProjectileMovementComp->bRotationFollowsVelocity = true;
	m_ProjectileMovementComp->bShouldBounce = true;

	InitialLifeSpan = 5.f;
}

USphereComponent* AHMProjectile::GetCollisionComp() const
{
	return m_CollisionComp;
}

UProjectileMovementComponent* AHMProjectile::GetProjectileMovement() const
{
	return m_ProjectileMovementComp;
}

void AHMProjectile::BeginPlay()
{
	Super::BeginPlay();

	//	생성되고 0.3초후에 타겟 찾기.
	//	이건 너무 원거리용이 되어버려서 주석.
	//GetWorld()->GetTimerManager().SetTimer(m_TimeHandle, this, &AHMProjectile::MoveToTarget, 0.3f, false);
	MoveToTarget();
}

void AHMProjectile::ProjectileDestroy()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_ProjectileDestroyParticle, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_ProjectileDestorySound, GetActorLocation());

	Destroy();
}

void AHMProjectile::MoveToTarget()
{
	USceneComponent* TargetComp = FindHomingTarget();
	if (TargetComp == nullptr || TargetComp->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHMProjectile::MoveToTarget() : TargetComp is nullptr!"));
		return;
	}

	if (m_ProjectileMovementComp == nullptr || m_ProjectileMovementComp->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHMProjectile::MoveToTarget() : m_ProjectileMovementComp is nullptr!"));
		return;
	}

	m_ProjectileMovementComp->HomingTargetComponent = TargetComp;
	m_ProjectileMovementComp->bIsHomingProjectile = true;
}

USceneComponent* AHMProjectile::FindHomingTarget()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHMMonster::StaticClass(), FoundActors);

	if (FoundActors.Num() <= 0)
	{
		return nullptr;
	}
	for (int i = 0; i < FoundActors.Num(); ++i)
	{
		AHMMonster* GetMontser = Cast<AHMMonster>(FoundActors[i]);
		if (GetMontser == nullptr)
		{
			continue;
		}

		//	내적을 구해서
		//	1이면 0도, 0이면 90도, -1이면 180도
		//	적당히 0.7로 범위를 잡아준다
		float Angle = GetDotProductTo(GetMontser);
		if (Angle < m_CheckHomingAngle)
		{
			continue;
		}


		//	범위 안에서도 가까운 애를 먼저 셀렉트 한다.
		//FVector DirectionVector = GetTargetLocation() - GetMontser->GetTargetLocation();
		//float VectorSize = DirectionVector.Size();
		//if (VectorSize < m_TargetDistance)
		//{
		//	m_TargetDistance = VectorSize;
		//	m_TargetMonster = GetMontser;
		//}

		//	가까운애보다 에임이랑 가까운애로 변경.
		if (Angle > m_SaveAngle)
		{
			m_SaveAngle = Angle;
			m_TargetMonster = GetMontser;
		}
	}

	if (m_TargetMonster == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not find monster"));
		return nullptr;
	}

	return m_TargetMonster->GetStaticMeshComp();
}

void AHMProjectile::CallBack_OnHit(UPrimitiveComponent* InHitComp, AActor* InOtherActor
	, UPrimitiveComponent* InOtherComp, FVector InNormalImpulse, const FHitResult& InHit)
{
	m_RadialForceComp->FireImpulse();

	if (InOtherActor == nullptr || InOtherActor->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHMProjectile::CallBack_OnHit() : InOtherActor is nullptr!"));
		return;
	}

	//	벽에 맞으면 바로 삭제
	if (InOtherActor->ActorHasTag(TEXT("Wall")) == true)
	{
		if (InOtherActor->Tags[0] == TEXT("Wall"))
		{
			ProjectileDestroy();
		}
	}

	//	몬스터 맞았는지 체크
	AHMMonster* CastMonster = Cast<AHMMonster>(InOtherActor);
	if (CastMonster == nullptr)
	{
		return;
	}

	if (InOtherActor != this && InOtherComp != nullptr)
	{
		//	몬스터 체력 차감
		CastMonster->UnderAttack();

		//	총알 삭제
		ProjectileDestroy();
	}
}
