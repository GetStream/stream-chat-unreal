// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStatusWidget.h"
#include "CoreMinimal.h"

#include "NewChatChannelStatusWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UNewChatChannelStatusWidget final : public UChannelStatusWidget
{
    GENERATED_BODY()

private:
    virtual bool IsForChannel(const UChatChannel*) const override;
};
