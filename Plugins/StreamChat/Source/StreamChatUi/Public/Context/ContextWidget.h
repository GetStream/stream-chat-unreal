// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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
    virtual void OnSlotAdded(UPanelSlot*) override;
    virtual void OnSlotRemoved(UPanelSlot*) override;
    // End of UPanelWidget interface

    // UVisual interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    // End of UVisual interface

#if WITH_EDITOR
    // UWidget interface
    virtual const FText GetPaletteCategory() override;
    // End of UWidget interface
#endif

    /**
     * Inset the content by the platform's safe area, so that it clears a notch, a dynamic island or a
     * home indicator. iOS and Android report these insets, but nothing consumes them automatically:
     * UMG only honours them through a SafeZone widget or GetSafeZonePadding, so a widget has to ask.
     * Contexts that wrap a whole screen should ask; nested ones should not, or the inset is applied
     * more than once.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    bool bApplySafeAreaPadding = false;

protected:
    // UWidget interface
    virtual TSharedRef<SWidget> RebuildWidget() override;
    // End of UWidget interface

    TSharedPtr<SBox> MyBox;
};
