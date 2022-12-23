// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HomingMissile : ModuleRules
{
	public HomingMissile(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem" });
	}
}
