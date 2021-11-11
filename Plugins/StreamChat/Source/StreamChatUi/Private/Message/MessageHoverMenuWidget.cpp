#include "Message/MessageHoverMenuWidget.h"

#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"

void UMessageHoverMenuWidget::Setup(const FMessage& InMessage, const EMessageSide InSide)
{
    Message = InMessage;
    Side = InSide;

    Super::Setup();
}

void UMessageHoverMenuWidget::OnSetup()
{
    if (Side == EMessageSide::Me)
    {
        ButtonGroup->ReplaceChildAt(0, OptionsButton);
        ButtonGroup->ReplaceChildAt(1, ReactionButton);
    }
    else if (Side == EMessageSide::You)
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
    UReactionPickerWidget* Widget = CreateWidget<UReactionPickerWidget>(this, ReactionPickerWidgetClass);
    Widget->Setup(Message);
    static constexpr bool bFocusImmediately = true;

    // Position centered below button
    const TSharedRef<SWidget> SlateWidget = Widget->TakeWidget();
    const FGeometry AllottedGeometry = ReactionButton->GetCachedGeometry();
    const FSlateLayoutTransform Transform = AllottedGeometry.GetAccumulatedLayoutTransform();
    SlateWidget->SlatePrepass(Transform.GetScale());
    const FVector2D PopupSizeLocalSpace = SlateWidget->GetDesiredSize();
    const FVector2D LocalPopupOffset{
        -(PopupSizeLocalSpace.X / 2 - AllottedGeometry.GetLocalSize().X / 2), AllottedGeometry.GetLocalSize().Y};
    const FVector2D SummonLocation = TransformPoint(Transform, LocalPopupOffset);

    TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
        TakeWidget(),
        {},
        SlateWidget,
        SummonLocation,
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
        bFocusImmediately);
    ContextMenu->GetContent()->SetRenderTransform(FTransform2D{0.5f * Transform.GetScale()});
    ContextMenu->GetContent()->
}
