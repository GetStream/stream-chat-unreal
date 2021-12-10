// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/AvatarWidget.h"
#include "Blueprint/UserWidget.h"
#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "HeaderWidget.h"
#include "OnlineStatusSubheaderWidget.h"
#include "TypingIndicatorWidget.h"

#include "MessageListHeaderWidget.generated.h"

class UChatChannel;

/**
 * @brief Top bar for a message list
 * @see https://www.figma.com/file/ekifwChR9tR7zRJg1QEzSM/Chat-Design-Kit---Current-version?node-id=15271%3A237867
 */
UCLASS()
class STREAMCHATUI_API UMessageListHeaderWidget final : public UStreamWidget
{
    GENERATED_BODY()

protected:
    virtual bool WantsChannel() override
    {
        return true;
    }
    virtual void OnChannel(UChatChannel*) override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UHeaderWidget* Header;

    UPROPERTY(meta = (BindWidget))
    UAvatarWidget* Avatar;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UTypingIndicatorWidget> TypingIndicatorWidgetClass = UTypingIndicatorWidget::StaticClass();
    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UOnlineStatusSubheaderWidget> StatusWidgetClass = UOnlineStatusSubheaderWidget::StaticClass();

private:
    UFUNCTION()
    void OnTypingIndicator(ETypingIndicatorState TypingState, const FUserRef& User);

    void ShowOnlineStatusSubheader();

    UPROPERTY(Transient)
    UTypingIndicatorWidget* TypingIndicator;
};
