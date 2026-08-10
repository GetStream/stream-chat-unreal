// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ContextMenu/ContextMenuAction.h"
#include "CoreMinimal.h"

#include "ThreadReplyContextMenuAction.generated.h"

/**
 * @brief Message action which opens the message's thread to reply in it
 *
 * Added to the action list in C++ rather than configured in WBP_ContextMenu, which predates threads.
 */
UCLASS()
class STREAMCHATUI_API UThreadReplyContextMenuAction final : public UContextMenuAction
{
    GENERATED_BODY()

public:
    UThreadReplyContextMenuAction();

protected:
    virtual void OnPerform(const FMessage&, UWidget* OwningWidget) override;
    virtual bool OnShouldDisplay(EMessageSide, const FMessage&) const override;
};
