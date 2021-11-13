// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/AvatarWidget.h"
#include "Channel/ChatChannel.h"
#include "Components/Button.h"
#include "CoreMinimal.h"
#include "Message/TimestampWidget.h"
#include "StreamUserWidget.h"

#include "ChannelStatusWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelStatusWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UChatChannel* InChannel);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void UpdateSelection(UChatChannel* SelectedChannel) const;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChannelStatusButtonClicked, UChatChannel*, Channel);
    UPROPERTY(BlueprintAssignable)
    FChannelStatusButtonClicked OnChannelStatusButtonClicked;

    DECLARE_MULTICAST_DELEGATE_OneParam(FChannelStatusButtonClickedNative, const UChatChannel*);
    FChannelStatusButtonClickedNative OnChannelStatusButtonClickedNative;

protected:
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

    UPROPERTY(EditAnywhere, Category = Defaults)
    FButtonStyle NormalStyle;
    UPROPERTY(EditAnywhere, Category = Defaults)
    FButtonStyle SelectedStyle;

private:
    virtual void OnSetup() override;

    virtual int32 NativePaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;

    void UpdateDynamic() const;
    void UpdateChannelTitleText() const;
    void UpdateRecentMessageText() const;

    UFUNCTION() void OnMessagesUpdated(const TArray<FMessage>& Messages);

    UFUNCTION()
    void OnButtonClicked();
    UPROPERTY(Transient)
    UChatChannel* Channel;

    mutable float RecentMessageAvailableSpace;
    mutable float ChannelTitleAvailableSpace;
};
