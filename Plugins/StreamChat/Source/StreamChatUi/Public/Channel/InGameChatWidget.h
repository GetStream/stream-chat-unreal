// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "Components/EditableText.h"
#include "CoreMinimal.h"
#include "FadingMessageListWidget.h"
#include "StreamWidget.h"
#include "Types/SlateEnums.h"

#include "InGameChatWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UInGameChatWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UChatChannel* InChannel);

protected:
    UPROPERTY(meta = (BindWidget))
    UFadingMessageListWidget* FadingMessageList;

private:
    virtual void OnSetup() override;
};
