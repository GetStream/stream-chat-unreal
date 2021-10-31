// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "StreamUserWidget.generated.h"

UCLASS(Abstract)
class STREAMCHATUI_API UStreamUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    /// This should be called in your Setup(...) function.
    /// All child widget initialization should be done in this function.
    /// Widget bindings, defaults and setup properties will be valid here.
    /// Only called once when widget is spawned, NOT when added to parent/viewport
    virtual void OnSetup() PURE_VIRTUAL(UStreamUserWidget::OnSetup, );
};
