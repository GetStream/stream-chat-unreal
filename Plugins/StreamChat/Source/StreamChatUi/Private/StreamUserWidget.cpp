#include "StreamUserWidget.h"

void UStreamUserWidget::Setup()
{
    OnSetup();
}

bool UStreamUserWidget::Initialize()
{
    if (Super::Initialize())
    {
        if (IsDesignTime())
        {
            OnSetup();
        }
        return true;
    }

    return false;
}
