// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "ReactionsBubbleWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UReactionsBubbleWidget final : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UNamedSlot* ReactionsSlot;
};
