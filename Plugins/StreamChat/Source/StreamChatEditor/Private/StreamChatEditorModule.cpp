// Copyright Epic Games, Inc. All Rights Reserved.

#include "StreamChatEditorModule.h"

#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Modules/ModuleManager.h"
#include "StreamChatSettings.h"

#define LOCTEXT_NAMESPACE "StreamChatEditor"

constexpr auto SettingsCategoryName = "StreamChat";
constexpr auto SettingsModuleName = "Settings";

void FStreamChatEditorModule::StartupModule()
{
    RegisterSettings();
}

void FStreamChatEditorModule::ShutdownModule()
{
    if (UObjectInitialized())
    {
        UnregisterSettings();
    }
}

void FStreamChatEditorModule::RegisterSettings()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(SettingsModuleName))
    {
        // Create the new category
        const ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

        SettingsContainer->DescribeCategory(
            SettingsCategoryName,
            LOCTEXT("SettingsCategoryName", "Stream Chat"),
            LOCTEXT("SettingsCategoryDescription", "Settings for the Stream Chat plugin"));

        // Register the settings
        const ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings(
            "Project",
            SettingsCategoryName,
            "General",
            LOCTEXT("GeneralSettingsSectionName", "General"),
            LOCTEXT("GeneralSettingsSectionDescription", "General settings for the Stream Chat plugin"),
            GetMutableDefault<UStreamChatSettings>());
    }
}

void FStreamChatEditorModule::UnregisterSettings()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(SettingsModuleName))
    {
        SettingsModule->UnregisterSettings("Project", SettingsCategoryName, "General");
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStreamChatEditorModule, StreamChatEditor)
