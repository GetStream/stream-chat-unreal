// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ContextMenu/ContextMenuAction.h"
#include "CoreMinimal.h"

#include "DeleteMessageContextMenuAction.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UDeleteMessageContextMenuAction final : public UContextMenuAction
{
    GENERATED_BODY()

protected:
    virtual void OnPerform(const FMessage&, UWidget* OwningWidget) override;
    virtual bool OnShouldDisplay(EMessageSide, const FMessage&) const override;
};
