// Copyright Epic Games, Inc. All Rights Reserved.

#include "HMMonster.h"

#include "HomingMissile/Monster/HMAIController.h"

AHMMonster::AHMMonster()
{
	m_StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	m_StaticMeshComp->SetupAttachment(RootComponent);

	AIControllerClass = AHMAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UStaticMeshComponent* AHMMonster::GetStaticMeshComp() const
{
	return m_StaticMeshComp;
}

void AHMMonster::UnderAttack()
{
	--m_HP;

	if (m_HP <= 0)
	{
		Destroy();
	}
}
