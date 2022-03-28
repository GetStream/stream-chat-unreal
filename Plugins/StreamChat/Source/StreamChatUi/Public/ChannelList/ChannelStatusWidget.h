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
UCLASS(Abstract)
class STREAMCHATUI_API UChannelStatusWidget : public UStreamWidget
{
    GENERATED_BODY()

public:
    UChannelStatusWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UChatChannel* InChannel);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void UpdateSelection(UChatChannel* SelectedChannel) const;

    virtual bool IsForChannel(const UChatChannel*) const;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChannelStatusButtonClicked, UChatChannel*, InChannel);
    UPROPERTY(BlueprintAssignable)
    FChannelStatusButtonClicked OnChannelStatusButtonClicked;

    DECLARE_MULTICAST_DELEGATE_OneParam(FChannelStatusButtonClickedNative, const UChatChannel*);
    FChannelStatusButtonClickedNative OnChannelStatusButtonClickedNative;

protected:
    virtual void OnSetup() override;
    virtual void OnTheme() override;

    UPROPERTY(meta = (BindWidget))
    UButton* Button;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TitleTextBlock;

    UPROPERTY(meta = (BindWidget))
    UImage* Divider;

private:
    virtual FLinearColor GetTitleColor(UThemeDataAsset*);

    UFUNCTION()
    void OnButtonClicked();

    FButtonStyle NormalStyle;
    FButtonStyle SelectedStyle;
};
