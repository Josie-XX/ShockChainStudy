// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShockChainStudy : ModuleRules
{
	public ShockChainStudy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Niagara"
		});

		PublicIncludePaths.AddRange(new string[] {
			"ShockChainStudy",
			"ShockChainStudy/Variant_Horror",
			"ShockChainStudy/Variant_Horror/UI",
			"ShockChainStudy/Variant_Shooter",
			"ShockChainStudy/Variant_Shooter/AI",
			"ShockChainStudy/Variant_Shooter/UI",
			"ShockChainStudy/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
