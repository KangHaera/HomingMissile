// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HMCharacter.generated.h"

class UInputComponent;
class USceneComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UMotionControllerComponent;

class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class HOMINGMISSILE_API AHMCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	struct ProjectileData
	{
		ProjectileData()
			: bIsPress(false)
			, FingerIndex(ETouchIndex::Type::Touch1)
		{ }

		bool bIsPress;
		const ETouchIndex::Type FingerIndex;
	};

public:
	AHMCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

private:
#pragma region actor action
	void OnFire();
	void ForwardMove(float InValue);
	void RightMove(float InValue);

	void TurnAtRate(float InRate);
	void LookUpAtRate(float InRate);

	void BeginTouch(const ETouchIndex::Type InFingerIndex, const FVector InLocation);
	void EndTouch(const ETouchIndex::Type InFingerIndex, const FVector InLocation);
#pragma endregion actor action


protected:
#pragma region component variable
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* m_SkeletalMeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* m_GunComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* m_MuzzleLocationComp;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_FirstPersonCameraComp;
#pragma endregion component variable


#pragma region camera setting variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float m_BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float m_BaseLookUpRate;
#pragma endregion camera setting variable


#pragma region projectile variable
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
		TSubclassOf<class AHMProjectile> m_ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
		USoundBase* m_FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* m_FireAnimation;
#pragma endregion projectile variable


private:	
#pragma region projectile data
	ProjectileData	m_ProjectileItem;
#pragma region projectile data

};

