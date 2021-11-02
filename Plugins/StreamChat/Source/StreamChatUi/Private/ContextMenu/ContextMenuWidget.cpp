#include "ContextMenu/ContextMenuWidget.h"

void UContextMenuWidget::Setup(const FMessage& InMessage, const EMessageSide InSide)
{
    Message = InMessage;
    Side = InSide;

    Super::Setup();
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

    TArray<UContextMenuAction*> DisplayedActions = Actions.FilterByPredicate(
        [&](const UContextMenuAction* Action) { return Action->ShouldDisplay(Side, Message); });

    const int32 LastIndex = DisplayedActions.Num() - 1;
    for (int32 Index = 0; Index < DisplayedActions.Num(); ++Index)
    {
        const EContextMenuButtonPosition Position = Index == 0           ? EContextMenuButtonPosition::Top
                                                    : Index == LastIndex ? EContextMenuButtonPosition::Bottom
                                                                         : EContextMenuButtonPosition::Mid;
        AddButton(DisplayedActions[Index], Position);
    }
}
