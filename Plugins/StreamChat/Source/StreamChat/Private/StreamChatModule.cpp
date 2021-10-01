// Copyright Epic Games, Inc. All Rights Reserved.

#include "StreamChatModule.h"

#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "StreamChatSettings.h"

#define LOCTEXT_NAMESPACE "FStreamChatUnrealModule"

constexpr auto SettingsCategoryName = "StreamChat";
constexpr auto SettingsModuleName = "Settings";

void FStreamChatModule::StartupModule()
{
	RegisterSettings();
}

void FStreamChatModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}

void FStreamChatModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(SettingsModuleName))
	{
		// Create the new category
		const ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

		SettingsContainer->DescribeCategory(SettingsCategoryName, LOCTEXT("SettingsCategoryName", "Stream Chat"),
			LOCTEXT("SettingsCategoryDescription", "Settings for the Stream Chat plugin"));

		// Register the settings
		const ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", SettingsCategoryName, "General",
			LOCTEXT("GeneralSettingsSectionName", "General"),
			LOCTEXT("GeneralSettingsSectionDescription", "General settings for the Stream Chat plugin"),
			GetMutableDefault<UStreamChatSettings>());
	}
}

void FStreamChatModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(SettingsModuleName))
	{
		SettingsModule->UnregisterSettings("Project", SettingsCategoryName, "General");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStreamChatModule, StreamChat)
