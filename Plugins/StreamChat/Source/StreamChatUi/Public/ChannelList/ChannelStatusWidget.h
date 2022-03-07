// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/AvatarWidget.h"
#include "Channel/ChatChannel.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "Message/TimestampWidget.h"
#include "StreamWidget.h"

#include "ChannelStatusWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelStatusWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UChannelStatusWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UChatChannel* InChannel);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void UpdateSelection(UChatChannel* SelectedChannel) const;

    bool IsForChannel(const UChatChannel*) const;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChannelStatusButtonClicked, UChatChannel*, InChannel);
    UPROPERTY(BlueprintAssignable)
    FChannelStatusButtonClicked OnChannelStatusButtonClicked;

    DECLARE_MULTICAST_DELEGATE_OneParam(FChannelStatusButtonClickedNative, const UChatChannel*);
    FChannelStatusButtonClickedNative OnChannelStatusButtonClickedNative;

protected:
    UPROPERTY(meta = (BindWidget))
    UChannelContextWidget* ChannelContextProvider;

    UPROPERTY(meta = (BindWidget))
    UButton* Button;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TitleTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RecentMessageTextBlock;

    UPROPERTY(meta = (BindWidget))
    UAvatarWidget* Avatar;

    UPROPERTY(meta = (BindWidget))
    UTimestampWidget* Timestamp;

    UPROPERTY(meta = (BindWidget))
    UWidget* Notification;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NotificationTextBlock;

    UPROPERTY(meta = (BindWidget))
    UImage* Divider;

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;

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
    void OnMessagesUpdated(const TArray<FMessage>& Messages);
    UFUNCTION()
    void OnUnreadChanged(int32 UnreadCount);

    UFUNCTION()
    void OnButtonClicked();

    mutable float RecentMessageAvailableSpace;
    mutable float ChannelTitleAvailableSpace;

    FButtonStyle NormalStyle;
    FButtonStyle SelectedStyle;
};
