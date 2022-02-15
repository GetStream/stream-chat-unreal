// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStatusWidget.h"
#include "Common/PaginateScrollWidget.h"
#include "CoreMinimal.h"

#include "ChannelListWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelListWidget final : public UPaginateScrollWidget
{
    GENERATED_BODY()

public:
    UChannelListWidget();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChannelStatusClicked, UChatChannel*, InChannel);
    UPROPERTY(BlueprintAssignable)
    FChannelStatusClicked OnChannelStatusClicked;

protected:
    virtual void Paginate(const EPaginationDirection Direction, const TFunction<void()> Callback) override;
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UChannelStatusWidget> ChannelStatusWidgetClass = UChannelStatusWidget::StaticClass();

private:
    virtual void OnClient() override;

    UFUNCTION()
    void ChannelStatusClicked(UChatChannel* ClickedChannel);
    UFUNCTION()
    void OnChannelsUpdated(const TArray<UChatChannel*>& InChannels);

    UPROPERTY(Transient)
    UChatChannel* CurrentChannel;
};
