#include "WidgetUtil.h"

UWidget* WidgetUtil::GetTypedParentWidget(UWidget* Widget, const TSubclassOf<UWidget> Type)

{
	UWidget* NextOuter = Widget;
	while (NextOuter && NextOuter->Slot)
	{
		NextOuter = NextOuter->Slot->Parent;
		if (NextOuter->IsA(Type))
		{
			return NextOuter;
		}
	}
	return nullptr;
}
