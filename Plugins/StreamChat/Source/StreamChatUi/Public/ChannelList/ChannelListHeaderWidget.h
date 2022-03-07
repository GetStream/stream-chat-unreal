// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Image.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "ChannelListHeaderWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelListHeaderWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UChannelListHeaderWidget();

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* Icon;

private:
    virtual void OnTheme() override;
};
