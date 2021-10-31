#include "Message/MessageHoverMenuWidget.h"

#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"

void UMessageHoverMenuWidget::Setup(const FMessage& InMessage, const EBubbleStackSide InSide)
{
    Message = InMessage;
    Side = InSide;

    OnSetup();
}

void UMessageHoverMenuWidget::OnSetup()
{
    if (Side == EBubbleStackSide::Me)
    {
        ButtonGroup->ReplaceChildAt(0, OptionsButton);
        ButtonGroup->ReplaceChildAt(1, ReactionButton);
    }
    else if (Side == EBubbleStackSide::You)
    {
        ButtonGroup->ReplaceChildAt(0, ReactionButton);
        ButtonGroup->ReplaceChildAt(1, OptionsButton);
    }

    OptionsButton->OnClicked.AddDynamic(this, &UMessageHoverMenuWidget::OnOptionsButtonClicked);
    ReactionButton->OnClicked.AddDynamic(this, &UMessageHoverMenuWidget::OnReactionButtonClicked);

    OptionsButton->WidgetStyle.NormalPadding = {};
    OptionsButton->WidgetStyle.PressedPadding = {};
    ReactionButton->WidgetStyle.NormalPadding = {};
    ReactionButton->WidgetStyle.PressedPadding = {};
}

void UMessageHoverMenuWidget::OnOptionsButtonClicked()
{
    UWidget* Widget = CreateWidget(this, ContextMenuWidgetClass);
    static constexpr bool bFocusImmediately = true;
    TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
        TakeWidget(),
        {},
        Widget->TakeWidget(),
        OptionsButton->GetCachedGeometry().GetAbsolutePosition(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
        bFocusImmediately);
}

void UMessageHoverMenuWidget::OnReactionButtonClicked()
{
    UWidget* Widget = CreateWidget(this, ReactionPickerWidgetClass);
    static constexpr bool bFocusImmediately = true;
    TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
        TakeWidget(),
        {},
        Widget->TakeWidget(),
        ReactionButton->GetCachedGeometry().GetAbsolutePosition(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
        bFocusImmediately);
}
