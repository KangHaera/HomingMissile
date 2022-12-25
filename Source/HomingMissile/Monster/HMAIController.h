// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HMAIController.generated.h"

/**
 * 
 */
UCLASS()
class HOMINGMISSILE_API AHMAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AHMAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	void OnRepeatTimer();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Value)
		float m_RepeatInterval = 3.f;;

private:
	FTimerHandle m_RepeatTimerHandle;
	
};
