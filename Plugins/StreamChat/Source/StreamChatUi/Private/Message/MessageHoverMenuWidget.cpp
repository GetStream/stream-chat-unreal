// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Message/MessageHoverMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/GridSlot.h"
#include "Editor/WidgetCompilerLog.h"
#include "WidgetUtil.h"

void UMessageHoverMenuWidget::Setup(const FMessage& InMessage, const EMessageSide InSide)
{
    Message = InMessage;
    Side = InSide;

    Super::Setup();
}

void UMessageHoverMenuWidget::OnSetup()
{
    if (OptionsMenuAnchor)
    {
        if (UGridSlot* GridSlot = Cast<UGridSlot>(OptionsMenuAnchor->Slot))
        {
            if (Side == EMessageSide::Me)
            {
                GridSlot->SetColumn(0);
            }
            else if (Side == EMessageSide::You)
            {
                GridSlot->SetColumn(1);
            }
        }
    }

    if (ReactionMenuAnchor)
    {
        if (UGridSlot* GridSlot = Cast<UGridSlot>(ReactionMenuAnchor->Slot))
        {
            if (Side == EMessageSide::Me)
            {
                GridSlot->SetColumn(1);
            }
            else if (Side == EMessageSide::You)
            {
                GridSlot->SetColumn(0);
            }
        }
    }

    if (OptionsButton)
    {
        OptionsButton->OnClicked.AddDynamic(this, &UMessageHoverMenuWidget::OnOptionsButtonClicked);

        OptionsButton->WidgetStyle.NormalPadding = {};
        OptionsButton->WidgetStyle.PressedPadding = {};
    }
    if (ReactionButton)
    {
        ReactionButton->OnClicked.AddDynamic(this, &UMessageHoverMenuWidget::OnReactionButtonClicked);

        ReactionButton->WidgetStyle.NormalPadding = {};
        ReactionButton->WidgetStyle.PressedPadding = {};
    }

    if (OptionsMenuAnchor)
    {
        OptionsMenuAnchor->OnGetUserMenuContentEvent.BindDynamic(this, &UMessageHoverMenuWidget::CreateOptionsMenu);
    }
    if (ReactionMenuAnchor)
    {
        ReactionMenuAnchor->OnGetUserMenuContentEvent.BindDynamic(this, &UMessageHoverMenuWidget::CreateReactionsMenu);
        ReactionMenuAnchor->SetPlacement(MenuPlacement_CenteredBelowAnchor);
    }
}

void UMessageHoverMenuWidget::OnOptionsButtonClicked()
{
    if (OptionsMenuAnchor)
    {
        OptionsMenuAnchor->Open(true);
        WidgetUtil::HideDefaultMenuBackground(OptionsMenuAnchor);
    }
}

void UMessageHoverMenuWidget::OnReactionButtonClicked()
{
    if (ReactionMenuAnchor)
    {
        ReactionMenuAnchor->Open(true);
        WidgetUtil::HideDefaultMenuBackground(ReactionMenuAnchor);
    }
}

UUserWidget* UMessageHoverMenuWidget::CreateOptionsMenu()
{
    UContextMenuWidget* Widget = CreateWidget<UContextMenuWidget>(this, ContextMenuWidgetClass);
    Widget->Setup(Message, Side);
    return Widget;
}

UUserWidget* UMessageHoverMenuWidget::CreateReactionsMenu()
{
    UReactionPickerWidget* Widget = CreateWidget<UReactionPickerWidget>(this, ReactionPickerWidgetClass);
    Widget->Setup(Message);
    return Widget;
}

#if WITH_EDITOR
void UMessageHoverMenuWidget::ValidateChild(const FName& Parent, const FName& Child, const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog)
{
    if (UPanelWidget* ParentWidget = BlueprintWidgetTree.FindWidget<UPanelWidget>(Parent))
    {
        int32 ChildIdx;
        UWidgetTree::FindWidgetChild(ParentWidget, Child, ChildIdx);

        if (ChildIdx == INDEX_NONE)
        {
            const FText MissingChildError = FText::FromString(TEXT("Bound widget \"{0}\" is expected to be a child of widget \"{1}\""));
            CompileLog.Error(FText::Format(MissingChildError, FText::FromName(Child), FText::FromName(Parent)));
        }
    }
}

void UMessageHoverMenuWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog) const
{
    {
        const FName Parent = GET_MEMBER_NAME_CHECKED(UMessageHoverMenuWidget, ReactionMenuAnchor);
        const FName Child = GET_MEMBER_NAME_CHECKED(UMessageHoverMenuWidget, ReactionButton);
        ValidateChild(Parent, Child, BlueprintWidgetTree, CompileLog);
    }
    {
        const FName Parent = GET_MEMBER_NAME_CHECKED(UMessageHoverMenuWidget, OptionsMenuAnchor);
        const FName Child = GET_MEMBER_NAME_CHECKED(UMessageHoverMenuWidget, OptionsButton);
        ValidateChild(Parent, Child, BlueprintWidgetTree, CompileLog);
    }
    Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);
}
#endif
