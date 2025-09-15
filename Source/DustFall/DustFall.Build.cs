// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DustFall : ModuleRules
{
	public DustFall(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "PhysicsCore", "AnimGraphRuntime",
			"OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils", "AdvancedSessions",
			"AdvancedSteamSessions", "Slate", "SlateCore", "RHI", "DLSSBlueprint", "ApplicationCore"
		});
	}
}