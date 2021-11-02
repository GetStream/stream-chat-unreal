#include "ContextMenu/ContextMenuWidget.h"

void UContextMenuWidget::Setup(const FMessage& InMessage, const EBubbleStackSide InSide)
{
    Message = InMessage;
    Side = InSide;
}

void UContextMenuWidget::AddButton(UContextMenuAction* Action, const EContextMenuButtonPosition Position)
{
    UContextMenuButtonWidget* Widget = CreateWidget<UContextMenuButtonWidget>(this, ContextMenuButtonWidgetClass);
    Widget->Setup(Message, Position, Action);
    ButtonsPanel->AddChildToVerticalBox(Widget);
}

void UContextMenuWidget::OnSetup()
{
    if (!ButtonsPanel)
    {
        return;
    }

    // Spawn buttons
    ButtonsPanel->ClearChildren();

    const TArray<UContextMenuAction*>& Actions = MenuActions.FindOrAdd(Side).Actions;
    const int32 LastIndex = Actions.Num() - 1;
    for (int32 Index = 0; Index < Actions.Num(); ++Index)
    {
        const EContextMenuButtonPosition Position = Index == 0           ? EContextMenuButtonPosition::Top
                                                    : Index == LastIndex ? EContextMenuButtonPosition::Bottom
                                                                         : EContextMenuButtonPosition::Mid;
        AddButton(Actions[Index], Position);
    }
}
