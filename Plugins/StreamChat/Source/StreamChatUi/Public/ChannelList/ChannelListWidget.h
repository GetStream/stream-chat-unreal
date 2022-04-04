// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStatusWidget.h"
#include "Common/PaginateScrollWidget.h"
#include "CoreMinimal.h"
#include "NewChatChannelStatusWidget.h"
#include "SummaryChannelStatusWidget.h"
#include "Team/NewChatWidget.h"

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
    virtual void NativeDestruct() override;
    virtual void Paginate(const EPaginationDirection Direction, const TFunction<void()> Callback) override;

    UPROPERTY(meta = (BindWidget))
    UImage* Divider;

    UPROPERTY(EditAnywhere, Category = Defaults)
    bool bAutoSelectFirstChannel = true;
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<USummaryChannelStatusWidget> ChannelStatusWidgetClass = USummaryChannelStatusWidget::StaticClass();
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UNewChatChannelStatusWidget> NewChatChannelStatusWidgetClass = UNewChatChannelStatusWidget::StaticClass();

private:
    virtual void OnClient() override;
    virtual void OnTheme() override;

    void RepopulateChannelList();
    bool IsNewChatActive() const;

    UFUNCTION()
    void OnChannelSelected(UChatChannel* ClickedChannel);
    UFUNCTION()
    void OnChannelsUpdated(const TArray<UChatChannel*>& InChannels);
    UFUNCTION()
    void OnBack();
    UFUNCTION()
    void OnNewChat();

    UPROPERTY(Transient)
    UChatChannel* CurrentChannel;

    UPROPERTY(Transient)
    UChatChannel* NewChatPreviousChannel;
};
