// Copyright Epic Games, Inc. All Rights Reserved.

#include "HMCharacter.h"

#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"

#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "XRMotionControllerBase.h" 
#include "MotionControllerComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"

#include "HomingMissile/Player/HMProjectile.h"


AHMCharacter::AHMCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(CapsuleComponentSizeX, CapsuleComponentSizeY);

	m_BaseTurnRate = 45.f;
	m_BaseLookUpRate = 45.f;

	//	카메라 생성
	m_FirstPersonCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	m_FirstPersonCameraComp->SetupAttachment(GetCapsuleComponent());
	m_FirstPersonCameraComp->SetRelativeLocation(FirstPersomCameraCompLocation); // Position the camera
	m_FirstPersonCameraComp->bUsePawnControlRotation = true;

	//	스켈레탈 메시 생성
	m_SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	m_SkeletalMeshComp->SetOnlyOwnerSee(true);
	m_SkeletalMeshComp->SetupAttachment(m_FirstPersonCameraComp);
	m_SkeletalMeshComp->bCastDynamicShadow = false;
	m_SkeletalMeshComp->CastShadow = false;
	m_SkeletalMeshComp->SetRelativeRotation(SkeletalMeshCompRotation);
	m_SkeletalMeshComp->SetRelativeLocation(SkeletalMeshCompLocation);

	//	총 생성
	m_GunComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	m_GunComp->SetOnlyOwnerSee(false);		
	m_GunComp->bCastDynamicShadow = false;
	m_GunComp->CastShadow = false;
	m_GunComp->SetupAttachment(RootComponent);

	//	총구 위치
	m_MuzzleLocationComp = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	m_MuzzleLocationComp->SetupAttachment(m_GunComp);
	m_MuzzleLocationComp->SetRelativeLocation(MuzzleLocationCompLocation);
}

void AHMCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (m_GunComp == nullptr || m_GunComp->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHMCharacter::BeginPlay() : m_GunComp is nullptr!"));
		return;
	}
	m_GunComp->AttachToComponent(m_SkeletalMeshComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));


	if (m_SkeletalMeshComp == nullptr || m_SkeletalMeshComp->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHMCharacter::BeginPlay() : m_SkeletalMeshComp is nullptr!"));
		return;
	}
	m_SkeletalMeshComp->SetHiddenInGame(false, true);
}

void AHMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHMCharacter::OnFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHMCharacter::ForwardMove);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHMCharacter::RightMove);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHMCharacter::TurnAtRate);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHMCharacter::LookUpAtRate);

	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AHMCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AHMCharacter::EndTouch);
	}
}

void AHMCharacter::OnFire()
{
	if (m_ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World == nullptr || World->IsValidLowLevel() == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AHMCharacter::OnFire() : World is nullptr!"));
			return;
		}
		if (m_MuzzleLocationComp == nullptr || m_MuzzleLocationComp->IsValidLowLevel() == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AHMCharacter::OnFire() : m_MuzzleLocationComp is nullptr!"));
			return;
		}

		const FRotator SpawnRotation = GetControlRotation();
		const FVector SpawnLocation = m_MuzzleLocationComp->GetComponentLocation();

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		World->SpawnActor<AHMProjectile>(m_ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}

	if (m_FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_FireSound, GetActorLocation());
	}

	if (m_FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = m_SkeletalMeshComp->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(m_FireAnimation, 1.f);
		}
	}
}

void AHMCharacter::BeginTouch(const ETouchIndex::Type InFingerIndex, const FVector InLocation)
{
	if (m_ProjectileItem.bIsPress == true)
	{
		return;
	}

	if (InFingerIndex == m_ProjectileItem.FingerIndex)
	{
		OnFire();
		m_ProjectileItem.bIsPress = true;
	}

}

void AHMCharacter::EndTouch(const ETouchIndex::Type InFingerIndex, const FVector InLocation)
{
	if (m_ProjectileItem.bIsPress == false)
	{
		return;
	}

	if (InFingerIndex == m_ProjectileItem.FingerIndex)
	{
		m_ProjectileItem.bIsPress = false;
	}
}

void AHMCharacter::ForwardMove(float InValue)
{
	if (InValue != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), InValue);
	}
}

void AHMCharacter::RightMove(float InValue)
{
	if (InValue != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), InValue);
	}
}

void AHMCharacter::TurnAtRate(float InRate)
{
	AddControllerYawInput(InRate * m_BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHMCharacter::LookUpAtRate(float InRate)
{
	AddControllerPitchInput(InRate * m_BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
