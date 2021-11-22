// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "Components/EditableText.h"
#include "CoreMinimal.h"
#include "FadingMessageListWidget.h"
#include "StreamUserWidget.h"
#include "Types/SlateEnums.h"

#include "InGameChatWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UInGameChatWidget final : public UStreamUserWidget
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

    UPROPERTY(Transient)
    UChatChannel* Channel;
};
