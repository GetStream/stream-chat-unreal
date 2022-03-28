// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "Components/Image.h"
#include "Context/ChannelContextWidget.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "NewChatWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UNewChatWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UNewChatWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup();

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;
};
