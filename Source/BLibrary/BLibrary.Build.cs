// Some copyright should be here...

using UnrealBuildTool;

public class BLibrary : ModuleRules
{
	public BLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			bEnableExceptions = true;
		}
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UMG",
				// ... add other public dependencies that you statically link with here ...
			}
			);
		
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ImageWrapper",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add("gdi32.lib");
			PublicAdditionalLibraries.Add("shell32.lib");
		}
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
