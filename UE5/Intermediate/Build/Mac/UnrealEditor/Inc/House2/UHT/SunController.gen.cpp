// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "House2/SunController.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSunController() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	HOUSE2_API UClass* Z_Construct_UClass_ASunController();
	HOUSE2_API UClass* Z_Construct_UClass_ASunController_NoRegister();
	UPackage* Z_Construct_UPackage__Script_House2();
// End Cross Module References
	void ASunController::StaticRegisterNativesASunController()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ASunController);
	UClass* Z_Construct_UClass_ASunController_NoRegister()
	{
		return ASunController::StaticClass();
	}
	struct Z_Construct_UClass_ASunController_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASunController_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_House2,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASunController_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "SunController.h" },
		{ "ModuleRelativePath", "SunController.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASunController_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASunController>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_ASunController_Statics::ClassParams = {
		&ASunController::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_ASunController_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ASunController_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASunController()
	{
		if (!Z_Registration_Info_UClass_ASunController.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASunController.OuterSingleton, Z_Construct_UClass_ASunController_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_ASunController.OuterSingleton;
	}
	template<> HOUSE2_API UClass* StaticClass<ASunController>()
	{
		return ASunController::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASunController);
	ASunController::~ASunController() {}
	struct Z_CompiledInDeferFile_FID_diogofouto_Documents_MEIC_A_P4_P3D_Labs_Assignment_1_P3D_proj_UE5_Source_House2_SunController_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_diogofouto_Documents_MEIC_A_P4_P3D_Labs_Assignment_1_P3D_proj_UE5_Source_House2_SunController_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_ASunController, ASunController::StaticClass, TEXT("ASunController"), &Z_Registration_Info_UClass_ASunController, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASunController), 2219860083U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_diogofouto_Documents_MEIC_A_P4_P3D_Labs_Assignment_1_P3D_proj_UE5_Source_House2_SunController_h_294385336(TEXT("/Script/House2"),
		Z_CompiledInDeferFile_FID_diogofouto_Documents_MEIC_A_P4_P3D_Labs_Assignment_1_P3D_proj_UE5_Source_House2_SunController_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_diogofouto_Documents_MEIC_A_P4_P3D_Labs_Assignment_1_P3D_proj_UE5_Source_House2_SunController_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
