// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HMProjectile.generated.h"

class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class URadialForceComponent;
class UProjectileMovementComponent;

class AHMMonster;
class USoundBase;
class UParticleSystem;

UCLASS(config=Game)
class HOMINGMISSILE_API AHMProjectile : public AActor
{
	GENERATED_BODY()


public:
	AHMProjectile();

#pragma region getter
	USphereComponent* GetCollisionComp() const;
	UProjectileMovementComponent* GetProjectileMovement() const;
#pragma endregion getter

protected:
	virtual void BeginPlay() override;

private:
	void ProjectileDestroy();

#pragma region find homming target
	void MoveToTarget();
	USceneComponent* FindHomingTarget();
#pragma endregion find homming target

#pragma region event
	UFUNCTION() void CallBack_OnHit(UPrimitiveComponent* InHitComp, AActor* InOtherActor
		, UPrimitiveComponent* InOtherComp, FVector InNormalImpulse, const FHitResult& InHit);
#pragma endregion event

protected:
#pragma region component variable
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
		USphereComponent* m_CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UStaticMeshComponent* m_StaticMeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		URadialForceComponent* m_RadialForceComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* m_ProjectileMovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UParticleSystem* m_ProjectileDestroyParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* m_ProjectileDestorySound;
				
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Value)
		float m_ProjectileCollisionSphereRadius = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Value)
		float m_ProjectileInitialSpeed = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Value)
		float m_ProjectileMaxSpeed = 3000.f;
#pragma endregion component variable

#pragma region set homing angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float m_CheckHomingAngle = 0.7;
#pragma region set homing angle

private:
	AHMMonster* m_TargetMonster = nullptr;

	float m_SaveAngle = -1.f;
	FTimerHandle m_TimeHandle;
};

