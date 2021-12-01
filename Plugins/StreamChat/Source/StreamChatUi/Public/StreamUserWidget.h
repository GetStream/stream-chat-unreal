// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "StreamUserWidget.generated.h"

UCLASS(Abstract, meta = (DisableNativeTick))
class STREAMCHATUI_API UStreamUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /// This must be called in your custom Setup(...) function.
    void Setup();

private:
    virtual bool Initialize() override;

    // Don't allow (pre)construction. This should all be done in OnSetup()
    virtual void NativeConstruct() override final
    {
    }
    virtual void NativePreConstruct() override final
    {
    }

    /// You should override this to perform all child widget initialization.
    /// Widget bindings, defaults and setup properties will be valid here.
    /// Only called once when widget is spawned, NOT when added to parent/viewport
    virtual void OnSetup()
    {
    }
};
