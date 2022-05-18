// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Common/SPaginateListWidget.h"
#include "Components/NativeWidgetHost.h"
#include "CoreMinimal.h"
#include "NewChatChannelStatusWidget.h"
#include "StreamWidget.h"
#include "SummaryChannelStatusWidget.h"
#include "Team/NewChatWidget.h"

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

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* Divider;

    UPROPERTY(meta = (BindWidget))
    UNativeWidgetHost* ListView;

    /// Number of additional channels to be queried when paginating
    UPROPERTY(EditAnywhere, Category = Pagination)
    int32 Limit = 10;

    UPROPERTY(EditAnywhere, Category = Defaults)
    bool bAutoSelectFirstChannel = true;
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<USummaryChannelStatusWidget> ChannelStatusWidgetClass = USummaryChannelStatusWidget::StaticClass();
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UNewChatChannelStatusWidget> NewChatChannelStatusWidgetClass = UNewChatChannelStatusWidget::StaticClass();

private:
    virtual void NativeDestruct() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

    virtual void OnClient() override;
    virtual void OnTheme() override;
    void Paginate(const EPaginationDirection Direction, const TFunction<void()> Callback);
    UWidget* CreateChannelWidget(UChatChannel* const&);

    void RebuildChannelList();

    UFUNCTION()
    void OnChannelSelected(UChatChannel* ClickedChannel);
    UFUNCTION()
    void OnChannelsUpdated(const TArray<UChatChannel*>& InChannels);
    UFUNCTION()
    void OnBack();

    UPROPERTY(Transient)
    UChatChannel* CurrentChannel;

    TSharedPtr<SPaginateListWidget<UChatChannel*>> PaginateListWidget;
};
