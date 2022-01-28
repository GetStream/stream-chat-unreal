// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "ReactionIconWidget.h"

#include "FontReactionIconWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UFontReactionIconWidget final : public UReactionIconWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

    UPROPERTY(EditAnywhere, Category = Defaults)
    TMap<FName, FString> Emoji;

private:
    virtual void OnSetup() override;
};
