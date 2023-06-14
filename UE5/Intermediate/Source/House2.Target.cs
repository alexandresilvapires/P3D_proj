using UnrealBuildTool;

public class House2Target : TargetRules
{
	public House2Target(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Game;
		ExtraModuleNames.Add("House2");
	}
}
