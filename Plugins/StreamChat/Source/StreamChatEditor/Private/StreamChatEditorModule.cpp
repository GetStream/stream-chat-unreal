// Copyright Epic Games, Inc. All Rights Reserved.

#include "StreamChatEditorModule.h"

#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "StreamChatSettings.h"
#include "StreamChatWebSocketSettings.h"

#define LOCTEXT_NAMESPACE "StreamChatEditor"

constexpr auto SettingsCategoryName = "StreamChat";
constexpr auto SettingsModuleName = "Settings";
constexpr auto GeneralSectionName = "General";
constexpr auto WebSocketSectionName = "WebSocket";

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
        SettingsModule->RegisterSettings(
            "Project",
            SettingsCategoryName,
            GeneralSectionName,
            LOCTEXT("GeneralSettingsSectionName", "General"),
            LOCTEXT("GeneralSettingsSectionDescription", "General settings for the Stream Chat plugin"),
            GetMutableDefault<UStreamChatSettings>());
        SettingsModule->RegisterSettings(
            "Project",
            SettingsCategoryName,
            WebSocketSectionName,
            LOCTEXT("WebSocketSettingsSectionName", "WebSocket"),
            LOCTEXT("WebSocketSettingsSectionDescription", "WebSocket settings for the Stream Chat plugin"),
            GetMutableDefault<UStreamChatWebSocketSettings>());
    }
}

void FStreamChatEditorModule::UnregisterSettings()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(SettingsModuleName))
    {
        SettingsModule->UnregisterSettings("Project", SettingsCategoryName, GeneralSectionName);
        SettingsModule->UnregisterSettings("Project", SettingsCategoryName, WebSocketSectionName);
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStreamChatEditorModule, StreamChatEditor)
