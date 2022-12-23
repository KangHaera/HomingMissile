// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "HMHUD.generated.h"

UCLASS()
class HOMINGMISSILE_API AHMHUD : public AHUD
{
	GENERATED_BODY()

public:
	AHMHUD();
	virtual void DrawHUD() override;

private:
	class UTexture2D* CrosshairTex;

};

