// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/AvatarWidget.h"
#include "Channel/ChatChannel.h"
#include "ChannelList/ChannelStatusWidget.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "Message/TimestampWidget.h"

#include "SummaryChannelStatusWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API USummaryChannelStatusWidget final : public UChannelStatusWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UChannelContextWidget* ChannelContextProvider;

    UPROPERTY(meta = (BindWidget))
    UAvatarWidget* Avatar;

    UPROPERTY(meta = (BindWidget))
    UTimestampWidget* Timestamp;

    UPROPERTY(meta = (BindWidget))
    UWidget* Notification;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NotificationTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RecentMessageTextBlock;

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;
    virtual bool IsForChannel(const UChatChannel*) const override;
    virtual FLinearColor GetTitleColor() override;

    virtual int32 NativePaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;

    void UpdateChannelTitleText() const;
    void UpdateRecentMessageText() const;

    UFUNCTION()
    void OnMessagesUpdated();
    UFUNCTION()
    void OnUnreadChanged(int32 UnreadCount);

    mutable float RecentMessageAvailableSpace;
    mutable float ChannelTitleAvailableSpace;
};
