// Fill out your copyright notice in the Description page of Project Settings.


#include "HMAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"



AHMAIController::AHMAIController()
{
}

void AHMAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	GetWorld()->GetTimerManager().SetTimer(m_RepeatTimerHandle, this, &AHMAIController::OnRepeatTimer, m_RepeatInterval, true);
}

void AHMAIController::OnUnPossess()
{
	Super::OnUnPossess();
	GetWorld()->GetTimerManager().ClearTimer(m_RepeatTimerHandle);
}

void AHMAIController::OnRepeatTimer()
{
	auto CurrentPawn = GetPawn();
	if (CurrentPawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AHMAIController::OnRepeatTimer() : CurrentPawn is nullptr!"));
		return;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
	if (NavSystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AHMAIController::OnRepeatTimer() : NavSystem is nullptr!"));
		return;
	}

	FNavLocation NextLocation;
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 1000.f, NextLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation);
	}
}
