// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ContextMenu/ContextMenuAction.h"
#include "CoreMinimal.h"

#include "CopyMessageContextMenuAction.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UCopyMessageContextMenuAction final : public UContextMenuAction
{
    GENERATED_BODY()

protected:
    virtual void OnPerform(const FMessage&) override;
};
