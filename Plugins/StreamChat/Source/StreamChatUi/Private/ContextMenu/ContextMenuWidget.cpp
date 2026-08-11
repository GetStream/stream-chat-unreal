// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/ContextMenuWidget.h"

#include "ContextMenu/ResendMessageContextMenuAction.h"
#include "ContextMenu/ThreadReplyContextMenuAction.h"

void UContextMenuWidget::Setup(const FMessage& InMessage, const EMessageSide InSide)
{
    Message = InMessage;
    Side = InSide;

    Super::Setup();
}

void UContextMenuWidget::SetHeaderContent(UWidget* Content)
{
    HeaderContent = Content;
}

template <class TAction>
void UContextMenuWidget::AddMissingAction()
{
    if (Actions.ContainsByPredicate([](const UContextMenuAction* Action) { return Action && Action->IsA<TAction>(); }))
    {
        return;
    }
    Actions.Insert(NewObject<TAction>(this), 0);
}

void UContextMenuWidget::AddButton(UContextMenuAction* Action, const EContextMenuButtonPosition Position)
{
    if (!ButtonsPanel)
    {
        return;
    }

    UContextMenuButtonWidget* Widget = CreateWidget<UContextMenuButtonWidget>(this, ContextMenuButtonWidgetClass);
    Widget->Setup(Message, Position, Action);
    ButtonsPanel->AddChildToVerticalBox(Widget);
}

void UContextMenuWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (!ButtonsPanel)
    {
        return;
    }

    // WBP_ContextMenu was authored before threads and moderation bounces existed and has no entry for
    // either, so they are added here rather than by editing the asset. Guarded so repeated
    // pre-constructs do not stack up copies, and kept out of the editor preview, which shows the
    // asset's own list.
    if (!IsDesignTime())
    {
        AddMissingAction<UThreadReplyContextMenuAction>();
        AddMissingAction<UResendMessageContextMenuAction>();
    }

    for (UContextMenuAction* Action : Actions)
    {
        Action->SetContext(GetClient(), GetChannel());
    }

    // Spawn buttons
    ButtonsPanel->ClearChildren();

    const TArray<UContextMenuAction*> DisplayedActions =
        IsDesignTime() ? Actions : Actions.FilterByPredicate([&](const UContextMenuAction* Action) { return Action->ShouldDisplay(Side, Message); });

    const int32 LastIndex = DisplayedActions.Num() - 1;
    for (int32 Index = 0; Index < DisplayedActions.Num(); ++Index)
    {
        const EContextMenuButtonPosition Position = Index == 0           ? EContextMenuButtonPosition::Top
                                                    : Index == LastIndex ? EContextMenuButtonPosition::Bottom
                                                                         : EContextMenuButtonPosition::Mid;
        AddButton(DisplayedActions[Index], Position);
    }

    // After the buttons, because they are added to a panel that was just cleared
    if (HeaderContent)
    {
        ButtonsPanel->InsertChildAt(0, HeaderContent);
    }
}
