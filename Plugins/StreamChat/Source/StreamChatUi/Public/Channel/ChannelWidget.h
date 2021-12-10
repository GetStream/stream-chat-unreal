// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "Context/ChannelContextWidget.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "ChannelWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UChatChannel* InChannel);

protected:
    UPROPERTY(meta = (BindWidget))
    UChannelContextWidget* ChannelContext;

private:
    virtual void OnSetup() override;
};
