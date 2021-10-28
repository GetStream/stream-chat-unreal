// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ContentWidget.h"
#include "CoreMinimal.h"
#include "Widgets/SWidget.h"

#include "ContextWidget.generated.h"

class SBox;

UCLASS(Abstract)
class STREAMCHATUI_API UContextWidget : public UContentWidget
{
    GENERATED_BODY()

public:
    UContextWidget();
    // UPanelWidget interface
    virtual void OnSlotAdded(UPanelSlot* Slot) override;
    virtual void OnSlotRemoved(UPanelSlot* Slot) override;
    // End of UPanelWidget interface

    // UVisual interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    // End of UVisual interface

#if WITH_EDITOR
    // UWidget interface
    virtual const FText GetPaletteCategory() override;
    // End of UWidget interface
#endif

    UFUNCTION(BlueprintPure, Category = "Stream Chat", meta = (DefaultToSelf = Widget, HidePin = Widget, DeterminesOutputType = Type))
    static UContextWidget* GetContext(UWidget* Widget, TSubclassOf<UContextWidget> Type);

protected:
    // UWidget interface
    virtual TSharedRef<SWidget> RebuildWidget() override;
    // End of UWidget interface

    TSharedPtr<SBox> MyBox;
};
