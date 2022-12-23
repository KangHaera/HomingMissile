// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HMMonster.generated.h"

class UStaticMeshComponent;


UCLASS(config=Game)
class AHMMonster : public ACharacter
{
	GENERATED_BODY()

public:
	AHMMonster();
	UStaticMeshComponent* GetStaticMeshComp() const;

	void UnderAttack();

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		UStaticMeshComponent* m_StaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int32 m_HP;

};

