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
	//	 �� ����
	m_CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	m_CollisionComp->InitSphereRadius(5.0f);
	m_CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	m_CollisionComp->OnComponentHit.AddDynamic(this, &AHMProjectile::CallBack_OnHit);
	m_CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	m_CollisionComp->CanCharacterStepUpOn = ECB_No;

	//	root�� ����
	RootComponent = m_CollisionComp;

	//	mesh ����
	m_StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	m_StaticMeshComp->SetupAttachment(RootComponent);

	//	��������� ����
	m_RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	m_RadialForceComp->SetupAttachment(RootComponent);

	//	������Ÿ�� �����Ʈ ����
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

	//	�����ǰ� 0.3���Ŀ� Ÿ�� ã��.
	//	�̰� �ʹ� ���Ÿ����� �Ǿ������ �ּ�.
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

		//	������ ���ؼ�
		//	1�̸� 0��, 0�̸� 90��, -1�̸� 180��
		//	������ 0.7�� ������ ����ش�
		float Angle = GetDotProductTo(GetMontser);
		if (Angle < m_CheckHomingAngle)
		{
			continue;
		}


		//	���� �ȿ����� ����� �ָ� ���� ����Ʈ �Ѵ�.
		//FVector DirectionVector = GetTargetLocation() - GetMontser->GetTargetLocation();
		//float VectorSize = DirectionVector.Size();
		//if (VectorSize < m_TargetDistance)
		//{
		//	m_TargetDistance = VectorSize;
		//	m_TargetMonster = GetMontser;
		//}

		//	�����ֺ��� �����̶� �����ַ� ����.
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

	//	���� ������ �ٷ� ����
	if (InOtherActor->ActorHasTag(TEXT("Wall")) == true)
	{
		if (InOtherActor->Tags[0] == TEXT("Wall"))
		{
			ProjectileDestroy();
		}
	}

	//	���� �¾Ҵ��� üũ
	AHMMonster* CastMonster = Cast<AHMMonster>(InOtherActor);
	if (CastMonster == nullptr)
	{
		return;
	}

	if (InOtherActor != this && InOtherComp != nullptr)
	{
		//	���� ü�� ����
		CastMonster->UnderAttack();

		//	�Ѿ� ����
		ProjectileDestroy();
	}
}
