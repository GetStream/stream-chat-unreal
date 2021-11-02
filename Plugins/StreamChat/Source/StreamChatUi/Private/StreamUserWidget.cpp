#include "StreamUserWidget.h"

void UStreamUserWidget::Setup()
{
    OnSetup();
    bSetupCalled = true;
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

void UStreamUserWidget::NativeConstruct()
{
    Super::NativeConstruct();
    ensureMsgf(
        bSetupCalled,
        TEXT(
            "You either forgot to call USuper::Setup() in your Setup(...) method, or you forgot to call Setup(...) after CreateWidget()"));
}
