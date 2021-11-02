#include "WidgetUtil.h"

UWidget* WidgetUtil::GetTypedParentWidget(UWidget* Widget, const TSubclassOf<UWidget> Type)

{
    UWidget* NextOuter = Widget;
    while (NextOuter)
    {
        if (NextOuter->IsA(Type))
        {
            return NextOuter;
        }

        if (NextOuter->Slot)
        {
            NextOuter = NextOuter->Slot->Parent;
            continue;
        }

        if (const UObject* WidgetTree = NextOuter->GetOuter())
        {
            if (UUserWidget* UserWidget = Cast<UUserWidget>(WidgetTree->GetOuter()))
            {
                NextOuter = UserWidget;
                continue;
            }
        }
        break;
    }
    return nullptr;
}
