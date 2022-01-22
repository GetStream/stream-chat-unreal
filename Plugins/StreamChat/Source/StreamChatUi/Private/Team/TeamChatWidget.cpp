// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Team/TeamChatWidget.h"

#include "ThemeDataAsset.h"

UTeamChatWidget::UTeamChatWidget()
{
    bWantsTheme = true;
}

void UTeamChatWidget::Setup(UStreamChatClientComponent* InClient)
{
    if (ClientContextWidget)
    {
        ClientContextWidget->Setup(InClient);
    }
}

void UTeamChatWidget::OnSetup()
{
    if (MessageListContainer)
    {
        MessageListContainer->SetPadding({0.f});
    }
}

void UTeamChatWidget::OnTheme(const UThemeDataAsset* Theme)
{
    if (MessageListContainer)
    {
        MessageListContainer->SetBrushColor(Theme->GetPaletteColor(Theme->MessageListContainerBackgroundColor));
    }
}
