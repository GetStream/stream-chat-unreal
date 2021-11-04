// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StreamUserWidget.h"
#include "Channel/ChatChannel.h"
#include "Context/ChannelContextWidget.h"
#include "ChannelWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelWidget : public UStreamUserWidget
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
    UPROPERTY(Transient)
    UChatChannel* Channel;
};
