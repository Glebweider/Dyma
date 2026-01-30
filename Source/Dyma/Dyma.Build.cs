// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Dyma : ModuleRules
{
	public Dyma(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "PhysicsCore", "AnimGraphRuntime",
			"OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils", "AdvancedSessions",
			"AdvancedSteamSessions", "Slate", "SlateCore", "RHI", "ApplicationCore",
			"HTTP", "Json", "JsonUtilities"
		});
	}
}