// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ContextMenu/ContextMenuAction.h"
#include "CoreMinimal.h"

#include "EditMessageContextMenuAction.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UEditMessageContextMenuAction final : public UContextMenuAction
{
    GENERATED_BODY()

protected:
    virtual void OnPerform(const FMessage&, UWidget* OwningWidget) override;
};
