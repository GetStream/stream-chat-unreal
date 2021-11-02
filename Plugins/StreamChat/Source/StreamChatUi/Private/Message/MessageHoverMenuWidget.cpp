#include "Message/MessageHoverMenuWidget.h"

#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"

void UMessageHoverMenuWidget::Setup(const FMessage& InMessage, const EBubbleStackSide InSide)
{
    Message = InMessage;
    Side = InSide;

    Super::Setup();
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
    const FVector2D Location = FSlateApplication::Get().GetCursorPos();
    UContextMenuWidget* Widget = CreateWidget<UContextMenuWidget>(this, ContextMenuWidgetClass);
    Widget->Setup(Message, Side);
    static constexpr bool bFocusImmediately = true;
    const TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
        TakeWidget(),
        {},
        Widget->TakeWidget(),
        Location,
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
        bFocusImmediately);
    ensure(ContextMenu.IsValid());
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
