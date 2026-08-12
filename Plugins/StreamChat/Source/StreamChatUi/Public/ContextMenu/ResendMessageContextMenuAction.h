// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ContextMenu/ContextMenuAction.h"
#include "CoreMinimal.h"

#include "ResendMessageContextMenuAction.generated.h"

/**
 * @brief Message action which sends a moderation-bounced message again, unchanged
 *
 * Added to the action list in C++ rather than configured in WBP_ContextMenu, which predates
 * moderation bounces.
 *
 * Only offered on the author's own bounced message. Sending the same text again will usually be
 * bounced again, so this sits alongside Edit rather than replacing it.
 */
UCLASS()
class STREAMCHATUI_API UResendMessageContextMenuAction final : public UContextMenuAction
{
    GENERATED_BODY()

public:
    UResendMessageContextMenuAction();

protected:
    virtual void OnPerform(const FMessage&, UWidget* OwningWidget) override;
    virtual bool OnShouldDisplay(EMessageSide, const FMessage&) const override;
};
