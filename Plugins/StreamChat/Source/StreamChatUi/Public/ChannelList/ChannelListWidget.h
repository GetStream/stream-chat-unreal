// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStatusWidget.h"
#include "Components/PanelWidget.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "ChannelListWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelListWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UChannelListWidget();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChannelStatusClicked, UChatChannel*, InChannel);
    UPROPERTY(BlueprintAssignable)
    FChannelStatusClicked OnChannelStatusClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* ChannelList;
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UChannelStatusWidget> ChannelStatusWidgetClass = UChannelStatusWidget::StaticClass();

private:
    virtual void OnClient() override;

    UFUNCTION()
    void ChannelStatusClicked(UChatChannel* ClickedChannel);
    UFUNCTION()
    void OnChannelsUpdated(const TArray<UChatChannel*>& InChannels);
};
