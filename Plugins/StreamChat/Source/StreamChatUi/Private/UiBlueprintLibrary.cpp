#include "UiBlueprintLibrary.h"

#include "Components/Widget.h"
#include "Framework/Application/IMenu.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Layout/WidgetPath.h"

void UUiBlueprintLibrary::AddContextMenu(UWidget* Widget, const FPointerEvent& InMouseEvent)
{
    // Right clicked, so summon a context menu if the cursor is within the widget
    const FWidgetPath WidgetPath =
        InMouseEvent.GetEventPath() != nullptr ? *InMouseEvent.GetEventPath() : FWidgetPath();

    if (const TSharedPtr<SWidget> MenuContentWidget = Widget->TakeWidget(); MenuContentWidget.IsValid())
    {
        static constexpr bool bFocusImmediately = true;
        TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
            InMouseEvent.GetWindow(),
            WidgetPath,
            MenuContentWidget.ToSharedRef(),
            InMouseEvent.GetScreenSpacePosition(),
            FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
            bFocusImmediately);
    }
}

void UUiBlueprintLibrary::DismissContextMenu(UWidget* Widget)
{
    FSlateApplication::Get().DismissMenuByWidget(Widget->TakeWidget());
}
